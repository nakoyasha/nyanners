#include "Camera.h"
#include "Application.h"
#include "core/Logger.h"
#include "services/RenderingService.h"

using namespace Nyanners::Instances;

namespace Nyanners::Scripting {
	auto registrator = ReflectionDescriptorRegistry::instance()->create_registrator([]() {
		  Services::ReflectionService::create_descriptor(
		    "Camera", {"Transformable"}
		  )
		    .add_property<
		      Camera,
		      glm::vec2,
		      &Camera::get_resolution,
		      &Camera::set_resolution>("Resolution", Vector2);
	  });
}

Camera::Camera() : Instance("Camera") {
}

void Camera::update(const float deltaTime) {
	if (!active) {
		return;
	}

	if (useDebugMovement && Services::RenderingService::renderer->currentWindow->hasFocus()) {
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
			position += cameraFront * velocity;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
			position -= cameraFront * velocity;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E))
			position += cameraUp * velocity;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
			position -= cameraUp * velocity;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
			position -= glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
			position += glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;

		// rebuild view
		view = glm::lookAt(position, position + cameraFront, cameraUp);
		rotation = {pitch, yaw, 0.0f };
		view = glm::rotate(view, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate(view, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		view = glm::rotate(view, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		Instance::update(deltaTime);
	}
}

void Camera::set_fov(unsigned int newFov) {
	fov = newFov;
	calculate_projection(lastSize, true);
}

void Camera::calculate_projection(const DataTypes::Vector2 &size, bool forceRecalculate = false) {
	if (forceRecalculate != true) {
		if (lastSize.x == size.x && lastSize.y == size.y) {
			return;
		}
	}

	// Core::Logger::log_debug(
	  // std::format("Camera size updated to {},{}", size.x, size.y)
	// );
	projection = glm::perspective(
	  glm::radians(static_cast<float>(fov)),
	  static_cast<float>(size.x) / static_cast<float>(size.y),
	  0.1f,
	  800000.0f
	);

	lastSize = size;
}

void Camera::set_position(const glm::vec3 &newPosition) {
	view = glm::lookAt(newPosition, newPosition + cameraFront, cameraUp);
	Transformable::set_position(newPosition);
}

glm::vec2 Camera::get_resolution() const {
	return this->resolution;
}

void Camera::set_resolution(const glm::vec2 &newResolution) {
	this->resolution = newResolution;
}
