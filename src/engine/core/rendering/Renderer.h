#pragma once
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Window.hpp"
#include "instances/Camera.h"
#include "instances/Instance.h"
#include "instances/drawable/Drawable.h"
#include "resources/FrameBuffer.h"

namespace Nyanners::Core {
	class Renderer {
	public:
		virtual ~Renderer() = default;
		Resources::FrameBuffer* framebuffer;
		std::shared_ptr<Instances::Camera> camera;

		virtual void initialize() = 0;
		virtual void start_frame() = 0;
		virtual void render(const std::shared_ptr<Instances::Instance> &instanceToRender) = 0;
		virtual void set_framerate_cap(const unsigned int framerate) = 0;
		virtual void bind_framebuffer(Resources::FrameBuffer* newFrameBuffer) = 0;
		virtual void set_current_camera(std::shared_ptr<Instances::Camera> newCamera) {
			camera = newCamera;
		}
		virtual void unbind_framebuffer() = 0;
		virtual void render_mesh(const Resources::Mesh* mesh) = 0;
		virtual void handle_event(const sf::Event* event) = 0;
		virtual void end_frame() = 0;
		virtual void shutdown() = 0;

		static std::unique_ptr<Renderer> create(sf::Window* window);
	};
}