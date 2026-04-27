#include "InputService.h"

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