#pragma once
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Window.hpp"
#include "instances/Instance.h"
#include "instances/drawable/Drawable.h"

namespace Nyanners::Services {
	class DebugUIService : public Instances::Instance {
	public:
		DebugUIService();
		~DebugUIService();

		void draw_imgui(const sf::Window *target);
		static void on_frame_end();
		static void handle_event(const sf::Window* target, const sf::Event* event);
	};
}