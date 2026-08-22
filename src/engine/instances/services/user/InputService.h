#pragma once
#include "SFML/Window/Event.hpp"
#include "instances/Instance.h"
#include "instances/basic/Signal.h"
#include "scripting/reflections/ReflectionDescriptorRegistry.h"

#include "InputObject.h"

namespace Nyanners::Input {
	struct InputEvent {
		const InputState state;
		const InputSource source;
		const KeyCode key;
	};
}

namespace Nyanners::Services {
	class InputService : public Instances::Instance, public Service<InputService> {
	public:
		std::shared_ptr<Instances::Signal<std::shared_ptr<Instances::InputObject>>> onInput;

		static std::shared_ptr<Instances::InputObject> make_input_object(const Input::InputState state, const Input::InputSource source, const Input::KeyCode keycode);

		InputService() : Instance("InputService") {
			onInput = std::make_shared<Instances::Signal<std::shared_ptr<Instances::InputObject>>>();
		};

		std::shared_ptr<Instances::SignalBase> get_on_input() const;
		void handle_event(const sf::Event *event) const;
		bool is_key_down(const Input::KeyCode &key);
		bool is_mouse_over_button(const glm::vec2 &buttonPosition, const glm::vec2 &buttonSize);
	};
}