#pragma once
#include "Renderer.h"
#include "OpenGLRenderer.h"

std::unique_ptr<Nyanners::Core::Renderer> Nyanners::Core::Renderer::create(sf::Window* window) {
	return std::make_unique<Nyanners::Core::OpenGLRenderer>(window);
}