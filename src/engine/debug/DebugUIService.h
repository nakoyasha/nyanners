#pragma once
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Window.hpp"
#include "instances/Instance.h"

namespace Nyanners::Services {
	class DebugUIService : public Instances::Instance {
	public:
		static bool renderWindows;
		bool demoWindowOpen = false;

		DebugUIService();
		~DebugUIService();

		void draw_imgui() const;
		bool get_demo_open() const;
		void set_demo_open(bool value);

		static void on_frame_end();
		static void handle_event(const sf::Window* target, const sf::Event* event);
	};
}