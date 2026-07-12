#include "Engine/Layers/input_manager.h"

#include "Engine/Core/app.h"
#include "Engine/Core/app_context.h"
#include "Engine/Core/logger.h"
#include "Engine/Layers/scene_manager.h"
#include "SDL3/SDL_joystick.h"
#include "SDL3/SDL_mouse.h"

#include <algorithm>
#include <cmath>

namespace Engine {
  InputManager::InputManager() {
    // Initialize connected gamepads
    int count;
    if (SDL_JoystickID *joysticks = SDL_GetGamepads(&count)) {
      for (int i = 0; i < count; ++i) {
        AddGamepad(joysticks[i]);
      }
      SDL_free(joysticks);
    }
  }

  InputManager::~InputManager() {
    for (auto &pair: m_gamepads) {
      SDL_CloseGamepad(pair.second);
    }
    m_gamepads.clear();
  }

  void InputManager::OnUpdate(float dt) {
    UpdateAccumulators();
    EvaluateActions();
  }

  void InputManager::LateUpdate() {
    ClearStateTransitions();
  }

  void InputManager::UpdateAccumulators() {
    m_mouseDeltaX = m_accumMouseDeltaX;
    m_mouseDeltaY = m_accumMouseDeltaY;
    m_scrollDeltaX = m_accumScrollX;
    m_scrollDeltaY = m_accumScrollY;

    m_accumMouseDeltaX = 0.0f;
    m_accumMouseDeltaY = 0.0f;
    m_accumScrollX = 0.0f;
    m_accumScrollY = 0.0f;

    m_inputText = m_accumInputText;
    m_accumInputText.clear();
  }

  void InputManager::ClearStateTransitions() {
    auto updateState = [](InputState &state) {
      if (state.pressed) {
        state.pressed = false;
        state.held = true;
      }
      if (state.released) {
        state.released = false;
        state.held = false;
      }
    };

    for (auto &pair: m_keyStates)
      updateState(pair.second);
    for (auto &pair: m_mouseStates)
      updateState(pair.second);
    for (auto &gamepadPair: m_gamepadButtonStates) {
      for (auto &btnPair: gamepadPair.second) {
        updateState(btnPair.second);
      }
    }
    for (auto &gamepadPair: m_actionStates) {
      for (auto &actPair: gamepadPair.second) {
        updateState(actPair.second);
      }
    }

    for (auto it = m_touches.begin(); it != m_touches.end();) {
      if (it->released) {
        it = m_touches.erase(it);
      } else {
        if (it->pressed) {
          it->pressed = false;
          it->held = true;
        }
        ++it;
      }
    }
  }

  void InputManager::EvaluateActions() {
    for (auto &gamepadPair: m_actionStates) {
      for (auto &actPair: gamepadPair.second) {
        bool isActive = false;
        float highestAxis = 0.0f;
        int gpadIdx = gamepadPair.first;

        auto it = m_actionBindings.find(actPair.first);
        if (it != m_actionBindings.end()) {
          for (const auto &chord: it->second) {
            bool chordActive = true;
            float chordAxis = 0.0f;

            for (const auto &binding: chord.bindings) {
              bool bindingActive = false;
              switch (binding.deviceType) {
                case InputDeviceType::Keyboard:
                  if (IsKeyHeld(std::get<KeyCode>(binding.code)) ||
                      IsKeyPressed(std::get<KeyCode>(binding.code)))
                    bindingActive = true;
                  break;
                case InputDeviceType::MouseButton: {
                  auto m_it =
                      m_mouseStates.find(std::get<MouseCode>(binding.code));
                  if (m_it != m_mouseStates.end() &&
                      (m_it->second.pressed || m_it->second.held))
                    bindingActive = true;
                }
                break;
                case InputDeviceType::GamepadButton: {
                  auto b_it = m_gamepadButtonStates[gpadIdx].find(
                    std::get<GamepadCode>(binding.code));
                  if (b_it != m_gamepadButtonStates[gpadIdx].end() &&
                      (b_it->second.pressed || b_it->second.held))
                    bindingActive = true;
                }
                break;
                case InputDeviceType::GamepadAxis: {
                  GamepadAxisCode axisCode =
                      std::get<GamepadAxisCode>(binding.code);
                  float axisVal = GetGamepadAxis(axisCode, gpadIdx);

                  if (axisCode == Gamepad::AxisLeftX ||
                      axisCode == Gamepad::AxisLeftY ||
                      axisCode == Gamepad::AxisRightX ||
                      axisCode == Gamepad::AxisRightY) {
                    float x = 0.0f, y = 0.0f;
                    if (axisCode == Gamepad::AxisLeftX ||
                        axisCode == Gamepad::AxisLeftY) {
                      x = GetGamepadAxis(Gamepad::AxisLeftX, gpadIdx);
                      y = GetGamepadAxis(Gamepad::AxisLeftY, gpadIdx);
                    } else {
                      x = GetGamepadAxis(Gamepad::AxisRightX, gpadIdx);
                      y = GetGamepadAxis(Gamepad::AxisRightY, gpadIdx);
                    }

                    float magnitude = std::sqrt(x * x + y * y);
                    if (magnitude >= binding.deadzone) {
                      bindingActive = true;

                      float normalizedMag = 0.0f;
                      if (binding.deadzone < 1.0f) {
                        normalizedMag =
                            std::clamp((magnitude - binding.deadzone) /
                                       (1.0f - binding.deadzone),
                                       0.0f, 1.0f);
                      }

                      float scaledAxis = (axisVal / magnitude) * normalizedMag;
                      if (std::abs(scaledAxis) > std::abs(chordAxis)) {
                        chordAxis = scaledAxis;
                      }
                    }
                  } else {
                    // Triggers don't need radial deadzones
                    if (std::abs(axisVal) >= binding.deadzone) {
                      bindingActive = true;
                      if (std::abs(axisVal) > std::abs(chordAxis)) {
                        chordAxis = axisVal;
                      }
                    }
                  }
                }
                break;
              }

              if (!bindingActive) {
                chordActive = false;
                break;
              }
            }

            if (chordActive && !chord.bindings.empty()) {
              isActive = true;
              if (std::abs(chordAxis) > std::abs(highestAxis))
                highestAxis = chordAxis;
            }
          }
        }

        auto &state = actPair.second;
        bool wasActive = state.pressed || state.held;

        if (isActive && !wasActive) {
          state.pressed = true;
          state.released = false;
          ActionPressedEvent pressedEvent(actPair.first, gpadIdx);
          App::GetLayer<SceneManager>().OnEvent(pressedEvent);
        } else if (!isActive && wasActive) {
          state.pressed = false;
          state.released = true;
          ActionReleasedEvent releasedEvent(actPair.first, gpadIdx);
          App::GetLayer<SceneManager>().OnEvent(releasedEvent);
        }

        state.axisValue = highestAxis;
      }
    }
  }

  bool InputManager::HandleEvent(SDL_Event *event) {
    switch (event->type) {
      case SDL_EVENT_KEY_DOWN: {
        if (!event->key.repeat) {
          GetKeyState(event->key.scancode).pressed = true;
          GetKeyState(event->key.scancode).released = false;
          GetKeyState(event->key.scancode).held = false;
        }
        break;
      }
      case SDL_EVENT_KEY_UP: {
        GetKeyState(event->key.scancode).released = true;
        GetKeyState(event->key.scancode).pressed = false;
        GetKeyState(event->key.scancode).held = false;
        break;
      }
      case SDL_EVENT_MOUSE_BUTTON_DOWN: {
        GetMouseState(event->button.button).pressed = true;
        GetMouseState(event->button.button).released = false;
        GetMouseState(event->button.button).held = false;
        break;
      }
      case SDL_EVENT_MOUSE_BUTTON_UP: {
        GetMouseState(event->button.button).released = true;
        GetMouseState(event->button.button).pressed = false;
        GetMouseState(event->button.button).held = false;
        break;
      }
      case SDL_EVENT_MOUSE_MOTION: {
        m_accumMouseDeltaX += event->motion.xrel;
        m_accumMouseDeltaY += event->motion.yrel;
        break;
      }
      case SDL_EVENT_MOUSE_WHEEL: {
        m_accumScrollX += event->wheel.x;
        m_accumScrollY += event->wheel.y;
        break;
      }
      case SDL_EVENT_TEXT_INPUT: {
        m_accumInputText += event->text.text;
        break;
      }
      case SDL_EVENT_GAMEPAD_ADDED: {
        AddGamepad(event->gdevice.which);
        break;
      }
      case SDL_EVENT_GAMEPAD_REMOVED: {
        RemoveGamepad(event->gdevice.which);
        break;
      }
      case SDL_EVENT_GAMEPAD_BUTTON_DOWN: {
        int gpadIdx = m_joystickToGamepadIndex[event->gbutton.which];
        GetGamepadButtonState(gpadIdx, (GamepadCode) event->gbutton.button)
            .pressed = true;
        GetGamepadButtonState(gpadIdx, (GamepadCode) event->gbutton.button)
            .released = false;
        GetGamepadButtonState(gpadIdx, (GamepadCode) event->gbutton.button)
            .held = false;
        break;
      }
      case SDL_EVENT_GAMEPAD_BUTTON_UP: {
        int gpadIdx = m_joystickToGamepadIndex[event->gbutton.which];
        GetGamepadButtonState(gpadIdx,
                              static_cast<GamepadCode>(event->gbutton.button))
            .released = true;
        GetGamepadButtonState(gpadIdx,
                              static_cast<GamepadCode>(event->gbutton.button))
            .pressed = false;
        GetGamepadButtonState(gpadIdx,
                              static_cast<GamepadCode>(event->gbutton.button))
            .held = false;
        break;
      }
      case SDL_EVENT_GAMEPAD_AXIS_MOTION: {
        int gpadIdx = m_joystickToGamepadIndex[event->gaxis.which];
        float val =
            event->gaxis.value / 32767.0f; // roughly normalize to -1.0 to 1.0
        m_gamepadAxes[gpadIdx]
            [static_cast<GamepadAxisCode>(event->gaxis.axis)] = val;
        break;
      }
      case SDL_EVENT_FINGER_DOWN: {
        Uint64 fingerId = event->tfinger.fingerID;
        bool found = false;
        for (auto &touch: m_touches) {
          if (touch.fingerID == fingerId) {
            touch.pressed = true;
            touch.released = false;
            touch.held = false;
            touch.position = Vec2f(event->tfinger.x, event->tfinger.y);
            found = true;
            break;
          }
        }
        if (!found) {
          TouchPoint touch;
          touch.fingerID = fingerId;
          touch.position = Vec2f(event->tfinger.x, event->tfinger.y);
          touch.pressed = true;
          m_touches.push_back(touch);
        }
        break;
      }
      case SDL_EVENT_FINGER_UP: {
        Uint64 fingerId = event->tfinger.fingerID;
        for (auto &touch: m_touches) {
          if (touch.fingerID == fingerId) {
            touch.released = true;
            touch.pressed = false;
            touch.held = false;
            touch.position = Vec2f(event->tfinger.x, event->tfinger.y);
            break;
          }
        }
        break;
      }
      case SDL_EVENT_FINGER_MOTION: {
        Uint64 fingerId = event->tfinger.fingerID;
        for (auto &touch: m_touches) {
          if (touch.fingerID == fingerId) {
            touch.position = Vec2f(event->tfinger.x, event->tfinger.y);
            break;
          }
        }
        break;
      }
      default: ;
    }
    return false;
  }

  void InputManager::AddGamepad(SDL_JoystickID joystickID) {
    if (m_gamepads.find(joystickID) != m_gamepads.end())
      return;
    SDL_Gamepad *gamepad = SDL_OpenGamepad(joystickID);
    if (gamepad) {
      m_gamepads[joystickID] = gamepad;
      int gpadIdx = m_nextGamepadIndex++;
      m_joystickToGamepadIndex[joystickID] = gpadIdx;
      ENGINE_LOG_INFO("Gamepad added: Index {}", gpadIdx);
      GamepadConnectedEvent connectedEvent(gpadIdx);
      App::GetLayer<SceneManager>().OnEvent(connectedEvent);

      // Initialize action states for this gamepad
      for (const auto &pair: m_actionBindings) {
        m_actionStates[gpadIdx][pair.first] = InputState{};
      }
    }
  }

  void InputManager::RemoveGamepad(SDL_JoystickID joystickID) {
    auto it = m_gamepads.find(joystickID);
    if (it != m_gamepads.end()) {
      SDL_CloseGamepad(it->second);
      m_gamepads.erase(it);
      int gpadIdx = m_joystickToGamepadIndex[joystickID];
      m_joystickToGamepadIndex.erase(joystickID);
      ENGINE_LOG_INFO("Gamepad removed: Index {}", gpadIdx);
      GamepadDisconnectedEvent disconnectedEvent(gpadIdx);
      App::GetLayer<SceneManager>().OnEvent(disconnectedEvent);
    }
  }

  InputManager::InputState &InputManager::GetKeyState(KeyCode key) {
    return m_keyStates[key];
  }

  InputManager::InputState &InputManager::GetMouseState(MouseCode button) {
    return m_mouseStates[button];
  }

  InputManager::InputState &InputManager::GetGamepadButtonState(
    int index, GamepadCode button) {
    return m_gamepadButtonStates[index][button];
  }

  void InputManager::BindAction(ActionID actionID, const InputChord &chord) {
    m_actionBindings[actionID].push_back(chord);
    // Ensure action state exists for gamepad 0 (keyboard/mouse primary)
    m_actionStates[0][actionID] = InputState{};
    for (const auto &pair: m_gamepads) {
      m_actionStates[m_joystickToGamepadIndex[pair.first]][actionID] =
          InputState{};
    }
  }

  void InputManager::UnbindAction(ActionID actionID) {
    m_actionBindings.erase(actionID);
  }

  void InputManager::ClearAllBindings() {
    m_actionBindings.clear();
  }

  bool InputManager::IsActionBound(ActionID actionID) const {
    auto it = m_actionBindings.find(actionID);
    return it != m_actionBindings.end() && !it->second.empty();
  }

  Vec2f InputManager::GetMousePosition() const {
    float x, y;
    SDL_GetMouseState(&x, &y);
    return Vec2f(x, y);
  }

  Vec2f InputManager::GetGlobalMousePosition() const {
    float x, y;
    SDL_GetGlobalMouseState(&x, &y);
    return Vec2f(x, y);
  }

  Vec2f InputManager::GetMouseDelta() const {
    return Vec2f(m_mouseDeltaX, m_mouseDeltaY);
  }

  Vec2f InputManager::GetMouseScrollDelta() const {
    return Vec2f(m_scrollDeltaX, m_scrollDeltaY);
  }

  void InputManager::SetCursorMode(CursorMode mode) const {
    SDL_Window *window = App::GetLayer<AppContext>().window;
    switch (mode) {
      case CursorMode::Normal:
        SDL_ShowCursor();
        SDL_SetWindowRelativeMouseMode(window, false);
        break;
      case CursorMode::Hidden:
        SDL_HideCursor();
        SDL_SetWindowRelativeMouseMode(window, false);
        break;
      case CursorMode::Locked:
        SDL_SetWindowRelativeMouseMode(window, true);
        break;
    }
  }

  void InputManager::SetCursorPosition(float x, float y) const {
    SDL_Window *window = App::GetLayer<AppContext>().window;
    SDL_WarpMouseInWindow(window, x, y);
  }

  void InputManager::StartTextInput() const {
    SDL_Window *window = App::GetLayer<AppContext>().window;
    SDL_StartTextInput(window);
  }

  void InputManager::StopTextInput() const {
    SDL_Window *window = App::GetLayer<AppContext>().window;
    SDL_StopTextInput(window);
  }

  std::string InputManager::GetInputText() const {
    return m_inputText;
  }

  int InputManager::GetTouchCount() const {
    return static_cast<int>(m_touches.size());
  }

  const TouchPoint *InputManager::GetTouch(int index) const {
    if (index >= 0 && index < m_touches.size()) {
      return &m_touches[index];
    }
    return nullptr;
  }

  const TouchPoint *InputManager::GetTouchByFingerID(Uint64 fingerID) const {
    for (const auto &touch: m_touches) {
      if (touch.fingerID == fingerID) {
        return &touch;
      }
    }
    return nullptr;
  }

  bool InputManager::IsActionPressed(ActionID actionID, int gamepadIndex)
  const {
    if (auto it1 = m_actionStates.find(gamepadIndex);
      it1 != m_actionStates.end()) {
      if (auto it2 = it1->second.find(actionID); it2 != it1->second.end())
        return it2->second.pressed;
    }
    return false;
  }

  bool InputManager::IsActionHeld(ActionID actionID, int gamepadIndex) const {
    if (auto it1 = m_actionStates.find(gamepadIndex);
      it1 != m_actionStates.end()) {
      if (auto it2 = it1->second.find(actionID); it2 != it1->second.end())
        return it2->second.held || it2->second.pressed;
    }
    return false;
  }

  bool InputManager::IsActionReleased(ActionID actionID, int gamepadIndex)
  const {
    if (auto it1 = m_actionStates.find(gamepadIndex);
      it1 != m_actionStates.end()) {
      if (auto it2 = it1->second.find(actionID); it2 != it1->second.end())
        return it2->second.released;
    }
    return false;
  }

  float InputManager::GetActionAxis(ActionID actionID, int gamepadIndex)
  const {
    if (auto it1 = m_actionStates.find(gamepadIndex);
      it1 != m_actionStates.end()) {
      if (auto it2 = it1->second.find(actionID); it2 != it1->second.end())
        return it2->second.axisValue;
    }
    return 0.0f;
  }

  bool InputManager::IsKeyPressed(KeyCode key) const {
    auto it = m_keyStates.find(key);
    return it != m_keyStates.end() && it->second.pressed;
  }

  bool InputManager::IsKeyHeld(KeyCode key) const {
    auto it = m_keyStates.find(key);
    return it != m_keyStates.end() && (it->second.held || it->second.pressed);
  }

  bool InputManager::IsKeyReleased(KeyCode key) const {
    auto it = m_keyStates.find(key);
    return it != m_keyStates.end() && it->second.released;
  }

  bool InputManager::IsModifierHeld(Modifier modifier) const {
    switch (modifier) {
      case Modifier::Shift:
        return IsKeyHeld(Key::LeftShift) || IsKeyHeld(Key::RightShift);
      case Modifier::Ctrl:
        return IsKeyHeld(Key::LeftCtrl) || IsKeyHeld(Key::RightCtrl);
      case Modifier::Alt:
        return IsKeyHeld(Key::LeftAlt) || IsKeyHeld(Key::RightAlt);
      case Modifier::GUI:
        return IsKeyHeld(Key::LeftGUI) || IsKeyHeld(Key::RightGUI);
    }
    return false;
  }

  bool InputManager::IsMouseButtonPressed(MouseCode button) const {
    auto it = m_mouseStates.find(button);
    return it != m_mouseStates.end() && it->second.pressed;
  }

  bool InputManager::IsMouseButtonHeld(MouseCode button) const {
    auto it = m_mouseStates.find(button);
    return it != m_mouseStates.end() && (it->second.held || it->second.pressed);
  }

  bool InputManager::IsMouseButtonReleased(MouseCode button) const {
    auto it = m_mouseStates.find(button);
    return it != m_mouseStates.end() && it->second.released;
  }

  bool InputManager::IsGamepadButtonPressed(GamepadCode button,
                                            int gamepadIndex) const {
    if (auto it1 = m_gamepadButtonStates.find(gamepadIndex);
      it1 != m_gamepadButtonStates.end()) {
      if (auto it2 = it1->second.find(button); it2 != it1->second.end())
        return it2->second.pressed;
    }
    return false;
  }

  bool InputManager::IsGamepadButtonHeld(GamepadCode button,
                                         int gamepadIndex) const {
    if (auto it1 = m_gamepadButtonStates.find(gamepadIndex);
      it1 != m_gamepadButtonStates.end()) {
      if (auto it2 = it1->second.find(button); it2 != it1->second.end())
        return it2->second.held || it2->second.pressed;
    }
    return false;
  }

  bool InputManager::IsGamepadButtonReleased(GamepadCode button,
                                             int gamepadIndex) const {
    if (auto it1 = m_gamepadButtonStates.find(gamepadIndex);
      it1 != m_gamepadButtonStates.end()) {
      if (auto it2 = it1->second.find(button); it2 != it1->second.end())
        return it2->second.released;
    }
    return false;
  }

  float InputManager::GetGamepadAxis(GamepadAxisCode axis, int gamepadIndex)
  const {
    if (auto it1 = m_gamepadAxes.find(gamepadIndex);
      it1 != m_gamepadAxes.end()) {
      if (auto it2 = it1->second.find(axis); it2 != it1->second.end())
        return it2->second;
    }
    return 0.0f;
  }

  void InputManager::RumbleGamepad(int gamepadIndex, float lowFrequency,
                                   float highFrequency, Uint32 durationMs)
  const {
    for (const auto &pair: m_joystickToGamepadIndex) {
      if (pair.second == gamepadIndex) {
        auto it = m_gamepads.find(pair.first);
        if (it != m_gamepads.end()) {
          SDL_RumbleGamepad(
            it->second,
            static_cast<Uint16>(std::clamp(lowFrequency, 0.0f, 1.0f) *
                                0xFFFF),
            static_cast<Uint16>(std::clamp(highFrequency, 0.0f, 1.0f) *
                                0xFFFF),
            durationMs);
        }
        break;
      }
    }
  }

  void InputManager::RumbleGamepadTriggers(int gamepadIndex, float leftRumble,
                                           float rightRumble,
                                           Uint32 durationMs) const {
    for (const auto &pair: m_joystickToGamepadIndex) {
      if (pair.second == gamepadIndex) {
        auto it = m_gamepads.find(pair.first);
        if (it != m_gamepads.end()) {
          SDL_RumbleGamepadTriggers(
            it->second,
            static_cast<Uint16>(std::clamp(leftRumble, 0.0f, 1.0f) *
                                0xFFFF),
            static_cast<Uint16>(std::clamp(rightRumble, 0.0f, 1.0f) *
                                0xFFFF),
            durationMs);
        }
        break;
      }
    }
  }

  void InputManager::StopGamepadRumble(int gamepadIndex) const {
    RumbleGamepad(gamepadIndex, 0.0f, 0.0f, 0);
    RumbleGamepadTriggers(gamepadIndex, 0.0f, 0.0f, 0);
  }
} // namespace Engine
