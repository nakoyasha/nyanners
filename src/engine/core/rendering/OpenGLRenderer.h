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
			void clear() override;
			void render(const std::shared_ptr<Instances::Instance> &instanceToRender) override;
			void bind_framebuffer(Resources::FrameBuffer* newFrameBuffer) override;
			void unbind_framebuffer() override;
			void calculate_projection(const DataTypes::Vector2& size) override;
			void render_mesh(const Resources::Mesh* mesh) override;
			void handle_event(const sf::Event* event) override;
			void set_depth_test(const Rendering::DepthCheckLevel& level) override;
			void set_previous_depth_test() override;
			void enable_depth_buffer() override;
			void disable_depth_buffer() override;

			void end_frame() override;
			void shutdown() override;

			DataTypes::Vector2 get_window_size() override;
			void set_window_size(const DataTypes::Vector2 newWindowSize) override;
		private:
			sf::Window* currentWindow;
			Rendering::DepthCheckLevel lastDepthLevel = Rendering::Less;
			void handle_error(const std::shared_ptr<Instances::Instance>& instanceWhereItHappened);
		};
}