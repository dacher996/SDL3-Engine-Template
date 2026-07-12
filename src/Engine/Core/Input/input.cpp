#include "Engine/Core/Input/input.h"
#include "Engine/Core/app.h"
#include "Engine/Layers/input_manager.h"

namespace Engine {
    static InputManager &GetInputManager() {
        return App::GetLayer<InputManager>();
    }

    bool Input::IsActionPressed(ActionID actionID, int gamepadIndex) {
        return GetInputManager().IsActionPressed(actionID, gamepadIndex);
    }

    bool Input::IsActionHeld(ActionID actionID, int gamepadIndex) {
        return GetInputManager().IsActionHeld(actionID, gamepadIndex);
    }

    bool Input::IsActionReleased(ActionID actionID, int gamepadIndex) {
        return GetInputManager().IsActionReleased(actionID, gamepadIndex);
    }

    float Input::GetActionAxis(ActionID actionID, int gamepadIndex) {
        return GetInputManager().GetActionAxis(actionID, gamepadIndex);
    }

    bool Input::IsKeyPressed(KeyCode key) {
        return GetInputManager().IsKeyPressed(key);
    }

    bool Input::IsKeyHeld(KeyCode key) {
        return GetInputManager().IsKeyHeld(key);
    }

    bool Input::IsKeyReleased(KeyCode key) {
        return GetInputManager().IsKeyReleased(key);
    }

    bool Input::IsModifierHeld(Modifier modifier) {
        return GetInputManager().IsModifierHeld(modifier);
    }

    bool Input::IsMouseButtonPressed(MouseCode button) {
        return GetInputManager().IsMouseButtonPressed(button);
    }
    
    bool Input::IsMouseButtonHeld(MouseCode button) {
        return GetInputManager().IsMouseButtonHeld(button);
    }

    bool Input::IsMouseButtonReleased(MouseCode button) {
        return GetInputManager().IsMouseButtonReleased(button);
    }

    bool Input::IsGamepadButtonPressed(GamepadCode button, int gamepadIndex) {
        return GetInputManager().IsGamepadButtonPressed(button, gamepadIndex);
    }

    bool Input::IsGamepadButtonHeld(GamepadCode button, int gamepadIndex) {
        return GetInputManager().IsGamepadButtonHeld(button, gamepadIndex);
    }

    bool Input::IsGamepadButtonReleased(GamepadCode button, int gamepadIndex) {
        return GetInputManager().IsGamepadButtonReleased(button, gamepadIndex);
    }

    float Input::GetGamepadAxis(GamepadAxisCode axis, int gamepadIndex) {
        return GetInputManager().GetGamepadAxis(axis, gamepadIndex);
    }

    Vec2f Input::GetMousePosition() {
        return GetInputManager().GetMousePosition();
    }

    Vec2f Input::GetGlobalMousePosition() {
        return GetInputManager().GetGlobalMousePosition();
    }

    Vec2f Input::GetMouseDelta() {
        return GetInputManager().GetMouseDelta();
    }

    Vec2f Input::GetMouseScrollDelta() {
        return GetInputManager().GetMouseScrollDelta();
    }

    void Input::SetCursorMode(CursorMode mode) {
        GetInputManager().SetCursorMode(mode);
    }

    void Input::SetCursorPosition(float x, float y) {
        GetInputManager().SetCursorPosition(x, y);
    }

    int Input::GetTouchCount() {
        return GetInputManager().GetTouchCount();
    }

    const TouchPoint* Input::GetTouch(int index) {
        return GetInputManager().GetTouch(index);
    }

    const TouchPoint* Input::GetTouchByFingerID(Uint64 fingerID) {
        return GetInputManager().GetTouchByFingerID(fingerID);
    }

    void Input::StartTextInput() {
        GetInputManager().StartTextInput();
    }

    void Input::StopTextInput() {
        GetInputManager().StopTextInput();
    }

    std::string Input::GetInputText() {
        return GetInputManager().GetInputText();
    }

    void Input::BindAction(ActionID actionID, const InputChord &chord) {
        GetInputManager().BindAction(actionID, chord);
    }

    void Input::UnbindAction(ActionID actionID) {
        GetInputManager().UnbindAction(actionID);
    }

    void Input::ClearAllBindings() {
        GetInputManager().ClearAllBindings();
    }

    bool Input::IsActionBound(ActionID actionID) {
        return GetInputManager().IsActionBound(actionID);
    }

    void Input::RumbleGamepad(int gamepadIndex, float lowFrequency, float highFrequency, Uint32 durationMs) {
        GetInputManager().RumbleGamepad(gamepadIndex, lowFrequency, highFrequency, durationMs);
    }

    void Input::RumbleGamepadTriggers(int gamepadIndex, float leftRumble, float rightRumble, Uint32 durationMs) {
        GetInputManager().RumbleGamepadTriggers(gamepadIndex, leftRumble, rightRumble, durationMs);
    }

    void Input::StopGamepadRumble(int gamepadIndex) {
        GetInputManager().StopGamepadRumble(gamepadIndex);
    }
}
