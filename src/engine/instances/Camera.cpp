#include "Camera.h"
#include "Application.h"
#include "core/Logger.h"
#include "services/RenderingService.h"

using namespace Nyanners::Instances;

Camera::Camera() : Instance("Camera") {
	// this->calculate_projection();
}

void Camera::update(const float deltaTime) {
	float velocity = moveSpeed * deltaTime;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
		pitch += 90.0f * deltaTime;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
		pitch -= 90.0f * deltaTime;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
		yaw -= 90.0f * deltaTime;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
		yaw += 90.0f * deltaTime;

	yaw = std::clamp(yaw, -180.0f, 180.0f);
	pitch = std::clamp(pitch, -180.0f, 180.0f);

	cameraFront = glm::normalize(
	  glm::vec3(
	    std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch)),
	    std::sin(glm::radians(pitch)),
	    std::sin(glm::radians(yaw) * std::cos(glm::radians(pitch)))
	  )
	);

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

void Camera::calculate_projection(const DataTypes::Vector2 &size) {
	if (lastSize.x == size.x && lastSize.y == size.y) {
		return;
	}

	Core::Logger::log(
	  std::format("Camera size updated to {},{}", size.x, size.y)
	);
	projection = glm::perspective(
	  glm::radians(45.0f),
	  static_cast<float>(size.x) / static_cast<float>(size.y),
	  0.1f,
	  100.0f
	);

	lastSize = size;
}
