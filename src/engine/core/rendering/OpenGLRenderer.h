#pragma once
#include "Renderer.h"
#include "SFML/Window/Context.hpp"
#include <map>

namespace Nyanners::Core {
		struct Character {
			unsigned int textureId;
			DataTypes::Vector2 size;
			DataTypes::Vector2 bearing;
			unsigned int nextGlyph;
		};

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
			void render_text(const std::string& text) override;
			void handle_event(const sf::Event* event) override;
			void end_frame() override;
			void shutdown() override;

			DataTypes::Vector2 get_window_size() override;
			void set_window_size(const DataTypes::Vector2 newWindowSize) override;
		private:
			sf::Window* currentWindow;

			std::map<char, Character> textCharacters;
			void generateCharacters();
			void handle_error(const std::shared_ptr<Instances::Instance>& instanceWhereItHappened);
		};
}