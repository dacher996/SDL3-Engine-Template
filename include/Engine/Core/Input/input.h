#ifndef ENGINE_INPUT_H
#define ENGINE_INPUT_H

#include "Engine/Core/Input/input_types.h"
#include "Engine/Core/datatypes.h"
#include <string>

namespace Engine {
    class Input {
    public:
        // Polling
        static bool IsActionPressed(ActionID actionID, int gamepadIndex = 0);

        static bool IsActionHeld(ActionID actionID, int gamepadIndex = 0);

        static bool IsActionReleased(ActionID actionID, int gamepadIndex = 0);

        static float GetActionAxis(ActionID actionID, int gamepadIndex = 0);

        static bool IsKeyPressed(KeyCode key);

        static bool IsKeyHeld(KeyCode key);

        static bool IsKeyReleased(KeyCode key);

        static bool IsModifierHeld(Modifier modifier);

        static bool IsMouseButtonPressed(MouseCode button);

        static bool IsMouseButtonHeld(MouseCode button);

        static bool IsMouseButtonReleased(MouseCode button);

        static bool IsGamepadButtonPressed(GamepadCode button, int gamepadIndex = 0);

        static bool IsGamepadButtonHeld(GamepadCode button, int gamepadIndex = 0);

        static bool IsGamepadButtonReleased(GamepadCode button, int gamepadIndex = 0);

        static float GetGamepadAxis(GamepadAxisCode axis, int gamepadIndex = 0);

        // Mouse & Cursor State
        static Vec2f GetMousePosition();

        static Vec2f GetGlobalMousePosition();

        static Vec2f GetMouseDelta();

        static Vec2f GetMouseScrollDelta();

        static void SetCursorMode(CursorMode mode);

        static void SetCursorPosition(float x, float y);

        // Touch State
        static int GetTouchCount();
        static const TouchPoint* GetTouch(int index);
        static const TouchPoint* GetTouchByFingerID(Uint64 fingerID);

        // Text Input
        static void StartTextInput();

        static void StopTextInput();

        static std::string GetInputText();

        // Binding Management
        static void BindAction(ActionID actionID, const InputChord &chord);

        static void UnbindAction(ActionID actionID);

        static void ClearAllBindings();

        static bool IsActionBound(ActionID actionID);

        // Rumble
        static void RumbleGamepad(int gamepadIndex, float lowFrequency, float highFrequency, Uint32 durationMs);

        static void RumbleGamepadTriggers(int gamepadIndex, float leftRumble, float rightRumble, Uint32 durationMs);

        static void StopGamepadRumble(int gamepadIndex);
    };
}

#endif //ENGINE_INPUT_H
