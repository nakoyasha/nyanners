#pragma once

#include <instances/basic/Object.h>

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
		Enter,

		Mouse0,
		Mouse1,
		Mouse2,
		Mouse3,
		Mouse4,
		Mouse5,
	};
}

namespace Nyanners::Instances {
	class InputObject : public Object {
	public:
		InputObject(const Input::InputState inputState, const Input::InputSource inputSource, const Input::KeyCode inputKeyCode) : Object("InputObject"), state(inputState), source(inputSource), keyCode(inputKeyCode) {};

		Input::InputState state {};
		Input::InputSource source {};
		Input::KeyCode keyCode = Input::KeyCode::Unknown;

		Input::KeyCode get_keycode() const;
		Input::InputState get_state() const;
	};
}
