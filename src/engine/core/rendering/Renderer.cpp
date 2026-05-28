#include "Renderer.h"
#include "OpenGLRenderer.h"

void Nyanners::Core::Renderer::set_viewport(Rendering::Viewport *newViewport) {
	currentViewport = newViewport;
}

void Nyanners::Core::Renderer::reset_viewport() {
	currentViewport = &this->defaultViewport;
}

std::unique_ptr<Nyanners::Core::Renderer> Nyanners::Core::Renderer::create(sf::Window* window) {
	return std::make_unique<OpenGLRenderer>(window);
}
