#pragma once
#include "Renderer.h"
#include "SFML/Graphics/RenderWindow.hpp"

namespace Nyanners::Core {
		class OpenGLRenderer : public Renderer {
			public:

			void initialize() override;
		private:
			sf::RenderWindow window;
		};
}