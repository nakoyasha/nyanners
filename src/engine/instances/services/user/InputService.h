#pragma once
#include "SFML/Window/Event.hpp"
#include "instances/Instance.h"
#include "instances/basic/Signal.h"

namespace Nyanners::Input {
	enum class InputState {
		Began = 0,
		Ended = 1,
		Changed = 2,
	};

	enum class InputSource {
		Mouse = 0,
		Keyboard = 6,
	};

	enum class KeyCode {
		Unknown,
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		One,
		Two,
		Three,
		Four,
		Five,
		Six,
		Seven,
		Eight,
		Nine,
		Zero,
		Backspace,
		Backquote,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		Space,
		LeftCtrl,
		RightCtrl,
		LeftAlt,
		RightAlt,
		Tab,
		LeftShift,
		RightShift,
	};

	struct InputEvent {
		const InputState state;
		const InputSource source;
		const KeyCode key;
	};
}

namespace Nyanners::Services {
	class InputService : public Instances::Instance {
	public:
		static Instances::Signal<Input::InputEvent> onInput;
		InputService() : Instance("InputService") {};

		static void handle_event(const sf::Event *event);
	};
}