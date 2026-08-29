#pragma once
#include "instances/Instance.h"
#include "resources/FrameBuffer.h"
#include "core/rendering/Viewport.h"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Window.hpp"

namespace Nyanners::Services {
	class DebugUIService : public Instances::Instance {
	public:
		static bool renderWindows;
		bool demoWindowOpen = false;

		Core::Rendering::Viewport* debugViewport {};
		Resources::FrameBuffer* viewportFramebuffer {};

		DebugUIService();
		~DebugUIService() override;

		void draw_imgui() const;
		bool get_demo_open() const;
		void set_demo_open(bool value);

		void add_standard_elements(Resources::FrameBuffer* framebuffer = nullptr);

		static void on_frame_end();
		static void handle_event(const sf::Window* target, const sf::Event* event);
	};
}
