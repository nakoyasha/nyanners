#include "InputObject.h"

#include "scripting/reflections/ReflectionDescriptorRegistry.h"
#include "instances/services/ReflectionService.h"

using namespace Nyanners::Instances;

namespace Nyanners::Scripting {
	static auto inputObjectRegistrator = ReflectionDescriptorRegistry::instance()->create_registrator([]() {
		// InputState
            Services::ReflectionService::register_enum("InputState", {
                {"Began", static_cast<int>(Input::InputState::Began)},
                {"Ended", static_cast<int>(Input::InputState::Ended)},
                {"Changed", static_cast<int>(Input::InputState::Changed)}
            });

            // InputSource
            Services::ReflectionService::register_enum("InputSource", {
                {"Mouse", static_cast<int>(Input::InputSource::Mouse)},
                {"Keyboard", static_cast<int>(Input::InputSource::Keyboard)}
            });

            // KeyCode
            Services::ReflectionService::register_enum("KeyCode", {
                {"Unknown", static_cast<int>(Input::KeyCode::Unknown)},
                {"A", static_cast<int>(Input::KeyCode::A)},
                {"B", static_cast<int>(Input::KeyCode::B)},
                {"C", static_cast<int>(Input::KeyCode::C)},
                {"D", static_cast<int>(Input::KeyCode::D)},
                {"E", static_cast<int>(Input::KeyCode::E)},
                {"F", static_cast<int>(Input::KeyCode::F)},
                {"G", static_cast<int>(Input::KeyCode::G)},
                {"H", static_cast<int>(Input::KeyCode::H)},
                {"I", static_cast<int>(Input::KeyCode::I)},
                {"J", static_cast<int>(Input::KeyCode::J)},
                {"K", static_cast<int>(Input::KeyCode::K)},
                {"L", static_cast<int>(Input::KeyCode::L)},
                {"M", static_cast<int>(Input::KeyCode::M)},
                {"N", static_cast<int>(Input::KeyCode::N)},
                {"O", static_cast<int>(Input::KeyCode::O)},
                {"P", static_cast<int>(Input::KeyCode::P)},
                {"Q", static_cast<int>(Input::KeyCode::Q)},
                {"R", static_cast<int>(Input::KeyCode::R)},
                {"S", static_cast<int>(Input::KeyCode::S)},
                {"T", static_cast<int>(Input::KeyCode::T)},
                {"U", static_cast<int>(Input::KeyCode::U)},
                {"V", static_cast<int>(Input::KeyCode::V)},
                {"W", static_cast<int>(Input::KeyCode::W)},
                {"X", static_cast<int>(Input::KeyCode::X)},
                {"Y", static_cast<int>(Input::KeyCode::Y)},
                {"Z", static_cast<int>(Input::KeyCode::Z)},

                {"One", static_cast<int>(Input::KeyCode::One)},
                {"Two", static_cast<int>(Input::KeyCode::Two)},
                {"Three", static_cast<int>(Input::KeyCode::Three)},
                {"Four", static_cast<int>(Input::KeyCode::Four)},
                {"Five", static_cast<int>(Input::KeyCode::Five)},
                {"Six", static_cast<int>(Input::KeyCode::Six)},
                {"Seven", static_cast<int>(Input::KeyCode::Seven)},
                {"Eight", static_cast<int>(Input::KeyCode::Eight)},
                {"Nine", static_cast<int>(Input::KeyCode::Nine)},
                {"Zero", static_cast<int>(Input::KeyCode::Zero)},

                {"Backspace", static_cast<int>(Input::KeyCode::Backspace)},
                {"Backquote", static_cast<int>(Input::KeyCode::Backquote)},

                {"F1", static_cast<int>(Input::KeyCode::F1)},
                {"F2", static_cast<int>(Input::KeyCode::F2)},
                {"F3", static_cast<int>(Input::KeyCode::F3)},
                {"F4", static_cast<int>(Input::KeyCode::F4)},
                {"F5", static_cast<int>(Input::KeyCode::F5)},
                {"F6", static_cast<int>(Input::KeyCode::F6)},
                {"F7", static_cast<int>(Input::KeyCode::F7)},
                {"F8", static_cast<int>(Input::KeyCode::F8)},
                {"F9", static_cast<int>(Input::KeyCode::F9)},
                {"F10", static_cast<int>(Input::KeyCode::F10)},
                {"F11", static_cast<int>(Input::KeyCode::F11)},
                {"F12", static_cast<int>(Input::KeyCode::F12)},

                {"Space", static_cast<int>(Input::KeyCode::Space)},
                {"LeftCtrl", static_cast<int>(Input::KeyCode::LeftCtrl)},
                {"RightCtrl", static_cast<int>(Input::KeyCode::RightCtrl)},
                {"LeftAlt", static_cast<int>(Input::KeyCode::LeftAlt)},
                {"RightAlt", static_cast<int>(Input::KeyCode::RightAlt)},
                {"Tab", static_cast<int>(Input::KeyCode::Tab)},
                {"LeftShift", static_cast<int>(Input::KeyCode::LeftShift)},
                {"RightShift", static_cast<int>(Input::KeyCode::RightShift)},

                {"Mouse0", static_cast<int>(Input::KeyCode::Mouse0)},
                {"Mouse1", static_cast<int>(Input::KeyCode::Mouse1)},
                {"Mouse2", static_cast<int>(Input::KeyCode::Mouse2)},
                {"Mouse3", static_cast<int>(Input::KeyCode::Mouse3)},
                {"Mouse4", static_cast<int>(Input::KeyCode::Mouse4)},
                {"Mouse5", static_cast<int>(Input::KeyCode::Mouse5)}
            });


		Services::ReflectionService::create_descriptor("InputObject", {"Instance"})
			.add_enum_property_chained<InputObject, Input::KeyCode, &InputObject::get_keycode>(
			  "KeyCode", "KeyCode"
			)
			.add_enum_property_chained<InputObject, Input::InputState, &InputObject::get_state>(
				"InputState", "InputState"
			);
	});
}

Nyanners::Input::KeyCode InputObject::get_keycode() const {
	return this->keyCode;
}

Nyanners::Input::InputState InputObject::get_state() const {
	return this->state;
}
