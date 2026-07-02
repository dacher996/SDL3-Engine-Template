#ifndef ENGINE_INPUT_TYPES_H
#define ENGINE_INPUT_TYPES_H

#include "Engine/Core/Input/key_codes.h"
#include "Engine/Core/Input/mouse_codes.h"
#include "Engine/Core/Input/gamepad_codes.h"
#include <variant>
#include <vector>

namespace Engine {
    using ActionID = Uint32;

    enum class InputDeviceType {
        Keyboard,
        MouseButton,
        GamepadButton,
        GamepadAxis
    };

    enum class CursorMode {
        Normal,
        Hidden,
        Locked
    };

    struct InputBinding {
        InputDeviceType deviceType;
        std::variant<KeyCode, MouseCode, GamepadCode, GamepadAxisCode> code;
        float deadzone = 0.1f; // For Gamepad Axis

        // Convenience constructors
        InputBinding(KeyCode key);
        InputBinding(MouseCode mouseBtn);
        InputBinding(GamepadCode gpBtn);
        InputBinding(GamepadAxisCode gpAxis, float axisDeadzone = 0.1f);
    };

    struct InputChord {
        std::vector<InputBinding> bindings;

        InputChord() = default;

        InputChord(const InputBinding &binding);

        InputChord(std::initializer_list<InputBinding> list);

        void AddBinding(const InputBinding &binding);
    };
}
#endif //ENGINE_INPUT_TYPES_H
