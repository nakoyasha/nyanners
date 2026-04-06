#pragma once
#include "Renderer.h"
#include "SFML/Window/Context.hpp"

namespace Nyanners::Core {
		class OpenGLRenderer : public Renderer {
			public:
			OpenGLRenderer(sf::Window* window);
			~OpenGLRenderer() override;

			void initialize() override;
			void start_frame() override;
			void render(const std::shared_ptr<Instances::Instance> &instanceToRender) override;
			void set_framerate_cap(const unsigned int framerate) override;
			void bind_framebuffer(Resources::FrameBuffer* newFrameBuffer) override;
			void unbind_framebuffer() override;
			void render_mesh(const Resources::Mesh* mesh) override;
			void handle_event(const sf::Event* event) override;
			void end_frame() override;
			void shutdown() override;
		private:
			sf::Window* currentWindow;
			void handle_error(const std::shared_ptr<Instances::Instance>& instanceWhereItHappened);
		};
}