#pragma once
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Window.hpp"
#include "instances/Instance.h"

namespace Nyanners::Services {
	class DebugUIService : public Instances::Instance {
	public:
		static bool renderWindows;

		DebugUIService();
		~DebugUIService();

		void draw_imgui() const;
		static void on_frame_end();
		static void handle_event(const sf::Window* target, const sf::Event* event);
	};
}