#include "Engine/Core/Input/input_types.h"

namespace Engine {

    InputBinding::InputBinding(KeyCode key) : deviceType(InputDeviceType::Keyboard), code(key) {
    }

    InputBinding::InputBinding(MouseCode mouseBtn) : deviceType(InputDeviceType::MouseButton),
                                                     code(mouseBtn) {
    }

    InputBinding::InputBinding(GamepadCode gpBtn) : deviceType(InputDeviceType::GamepadButton), code(gpBtn) {
    }

    InputBinding::InputBinding(GamepadAxisCode gpAxis, float axisDeadzone) : deviceType(InputDeviceType::GamepadAxis),
                                                                             code(gpAxis), deadzone(axisDeadzone) {
    }

    InputChord::InputChord(const InputBinding &binding) { bindings.push_back(binding); }

    InputChord::InputChord(std::initializer_list<InputBinding> list) : bindings(list) {
    }

    void InputChord::AddBinding(const InputBinding &binding) { bindings.push_back(binding); }

}
