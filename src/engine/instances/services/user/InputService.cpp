#include "InputService.h"

#include "imgui.h"
#include "instances/services/RenderingService.h"

using namespace Nyanners;
using namespace Nyanners::Services;

Instances::Signal<Input::InputEvent> InputService::onInput;

inline Input::KeyCode sfmlToUs(const sf::Keyboard::Key &key) {
	using K = sf::Keyboard::Key;

	switch (key) {
		case K::A:
			return Input::KeyCode::A;
		case K::B:
			return Input::KeyCode::B;
		case K::C:
			return Input::KeyCode::C;
		case K::D:
			return Input::KeyCode::D;
		case K::E:
			return Input::KeyCode::E;
		case K::F:
			return Input::KeyCode::F;
		case K::G:
			return Input::KeyCode::G;
		case K::H:
			return Input::KeyCode::H;
		case K::I:
			return Input::KeyCode::I;
		case K::J:
			return Input::KeyCode::J;
		case K::K:
			return Input::KeyCode::K;
		case K::L:
			return Input::KeyCode::L;
		case K::M:
			return Input::KeyCode::M;
		case K::N:
			return Input::KeyCode::N;
		case K::O:
			return Input::KeyCode::O;
		case K::P:
			return Input::KeyCode::P;
		case K::Q:
			return Input::KeyCode::Q;
		case K::R:
			return Input::KeyCode::R;
		case K::S:
			return Input::KeyCode::S;
		case K::T:
			return Input::KeyCode::T;
		case K::U:
			return Input::KeyCode::U;
		case K::V:
			return Input::KeyCode::V;
		case K::W:
			return Input::KeyCode::W;
		case K::X:
			return Input::KeyCode::X;
		case K::Y:
			return Input::KeyCode::Y;
		case K::Z:
			return Input::KeyCode::Z;

		case K::Num0:
			return Input::KeyCode::Zero;
		case K::Num1:
			return Input::KeyCode::One;
		case K::Num2:
			return Input::KeyCode::Two;
		case K::Num3:
			return Input::KeyCode::Three;
		case K::Num4:
			return Input::KeyCode::Four;
		case K::Num5:
			return Input::KeyCode::Five;
		case K::Num6:
			return Input::KeyCode::Six;
		case K::Num7:
			return Input::KeyCode::Seven;
		case K::Num8:
			return Input::KeyCode::Eight;
		case K::Num9:
			return Input::KeyCode::Nine;

		case K::Space:
			return Input::KeyCode::Space;
		case K::Tab:
			return Input::KeyCode::Tab;
		case K::LShift:
			return Input::KeyCode::LeftShift;
		case K::RShift:
			return Input::KeyCode::RightShift;
		case K::LControl:
			return Input::KeyCode::LeftCtrl;
		case K::RControl:
			return Input::KeyCode::RightCtrl;
		case K::LAlt:
			return Input::KeyCode::LeftAlt;
		case K::RAlt:
			return Input::KeyCode::RightAlt;
		case K::Backspace:
			return Input::KeyCode::Backspace;
		case K::Grave:
			return Input::KeyCode::Backquote;

		case K::F1:
			return Input::KeyCode::F1;
		case K::F2:
			return Input::KeyCode::F2;
		case K::F3:
			return Input::KeyCode::F3;
		case K::F4:
			return Input::KeyCode::F4;
		case K::F5:
			return Input::KeyCode::F5;
		case K::F6:
			return Input::KeyCode::F6;
		case K::F7:
			return Input::KeyCode::F7;
		case K::F8:
			return Input::KeyCode::F8;
		case K::F9:
			return Input::KeyCode::F9;
		case K::F10:
			return Input::KeyCode::F10;
		case K::F11:
			return Input::KeyCode::F11;
		case K::F12:
			return Input::KeyCode::F12;

		default:
			return Input::KeyCode::Unknown;
	}
}
inline sf::Keyboard::Key usToSFML(const Input::KeyCode& key) {
    using K = Input::KeyCode;
    using S = sf::Keyboard::Key;
	using M = sf::Mouse::Button;

    switch (key) {
        // Letters
        case K::A: return S::A;
        case K::B: return S::B;
        case K::C: return S::C;
        case K::D: return S::D;
        case K::E: return S::E;
        case K::F: return S::F;
        case K::G: return S::G;
        case K::H: return S::H;
        case K::I: return S::I;
        case K::J: return S::J;
        case K::K: return S::K;
        case K::L: return S::L;
        case K::M: return S::M;
        case K::N: return S::N;
        case K::O: return S::O;
        case K::P: return S::P;
        case K::Q: return S::Q;
        case K::R: return S::R;
        case K::S: return S::S;
        case K::T: return S::T;
        case K::U: return S::U;
        case K::V: return S::V;
        case K::W: return S::W;
        case K::X: return S::X;
        case K::Y: return S::Y;
        case K::Z: return S::Z;

        // Numbers
        case K::Zero: return S::Num0;
        case K::One: return S::Num1;
        case K::Two: return S::Num2;
        case K::Three: return S::Num3;
        case K::Four: return S::Num4;
        case K::Five: return S::Num5;
        case K::Six: return S::Num6;
        case K::Seven: return S::Num7;
        case K::Eight: return S::Num8;
        case K::Nine: return S::Num9;

        // Modifiers / controls
        case K::Space:      return S::Space;
        case K::Tab:        return S::Tab;
        case K::LeftShift:     return S::LShift;
        case K::RightShift:     return S::RShift;
        case K::LeftCtrl:   return S::LControl;
        case K::RightCtrl:   return S::RControl;
        case K::LeftAlt:       return S::LAlt;
        case K::RightAlt:       return S::RAlt;
        case K::Backspace:  return S::Backspace;
        case K::Backquote:      return S::Grave;

        // Function keys
        case K::F1: return S::F1;
        case K::F2: return S::F2;
        case K::F3: return S::F3;
        case K::F4: return S::F4;
        case K::F5: return S::F5;
        case K::F6: return S::F6;
        case K::F7: return S::F7;
        case K::F8: return S::F8;
        case K::F9: return S::F9;
        case K::F10: return S::F10;
        case K::F11: return S::F11;
        case K::F12: return S::F12;

        default:
            return S::Unknown;
    }
}
inline sf::Mouse::Button usToSFMLMouse(const Input::KeyCode& key) {
    using K = Input::KeyCode;
	using M = sf::Mouse::Button;

    switch (key) {
        // Letters
    	case K::Mouse0: return M::Left;
    	case K::Mouse1: return M::Right;
    	case K::Mouse2: return M::Middle;
    	case K::Mouse4: return M::Extra1;
    	case K::Mouse5: return M::Extra2;
    }
}


void InputService::handle_event(const sf::Event *event) {
	// TODO: mouse input and etc
	if (const auto *inputStarted = event->getIf<sf::Event::KeyPressed>()) {
		onInput.fire({
			.state = Input::InputState::Began,
			.source = Input::InputSource::Keyboard,
			.key = sfmlToUs(inputStarted->code)
		});
	} else if (const auto *inputEnded = event->getIf<sf::Event::KeyReleased>()) {
		onInput.fire({
			.state = Input::InputState::Ended,
			.source = Input::InputSource::Keyboard,
			.key = sfmlToUs(inputEnded->code)
		});
	}
}

bool InputService::is_key_down(const Input::KeyCode &key) {
	const auto keyDown = sf::Keyboard::isKeyPressed(usToSFML(key));

	if (keyDown != true) {
		return sf::Mouse::isButtonPressed(usToSFMLMouse(key));
	}

	return keyDown;
}

bool InputService::is_mouse_over_button(const glm::vec2 &buttonPosition, const glm::vec2 &buttonSize) {
	/* TODO: find a way to get viewport position out of imgui, this currently assumes there is no debug UI applied?
	 * implement a Viewport struct maybe
	 */
	const auto& viewport = RenderingService::renderer->currentViewport;
	const auto& mousePosition = sf::Mouse::getPosition(*RenderingService::renderer->currentWindow);

	const auto relativeMouse = glm::ivec2(mousePosition.x - (viewport->position.x),mousePosition.y - (viewport->position.y));

	const bool intersectsX = relativeMouse.x >= buttonPosition.x && relativeMouse.x <= buttonPosition.x + buttonSize.x;
	const bool intersectsY = relativeMouse.y >= buttonPosition.y && relativeMouse.y <= buttonPosition.y + buttonSize.y;

	return intersectsX && intersectsY;
}
