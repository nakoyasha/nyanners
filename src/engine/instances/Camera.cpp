#include "Camera.h"
#include "Application.h"
#include "services/RenderingService.h"

using namespace Nyanners::Instances;
Camera::Camera() : Instance("Camera") {
	this->calculate_projection();
}

void Camera::update(const float deltaTime) {
	this->calculate_projection();
	float velocity = moveSpeed * deltaTime;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
		cameraPos += cameraFront * velocity;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		cameraPos -= cameraFront * velocity;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E))
		cameraPos += cameraUp * velocity;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
		cameraPos -= cameraUp * velocity;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;

	// rebuild view
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	Instance::update(deltaTime);
}


void Camera::calculate_projection() {
	const auto renderService = Application::instance()->currentModel->get_service<Services::RenderingService>("RenderingService");
	const auto size = renderService->window->getSize();

	if (size.x == 0 || size.y == 0) {
		projection = glm::perspective(
			glm::radians(45.0f),
			static_cast<float>(1280) / static_cast<float>(720),
			0.1f,
			100.0f
		);
	} else {
		projection = glm::perspective(
			glm::radians(45.0f),
			static_cast<float>(size.x) / static_cast<float>(size.y),
			0.1f,
			100.0f
		);
	}

}