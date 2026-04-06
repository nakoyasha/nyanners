#include "Camera.h"
#include "Application.h"
#include "services/RenderingService.h"

using namespace Nyanners::Instances;
Camera::Camera() : Instance("Camera") {
	this->calculate_projection();
}

void Camera::update(const float deltaTime) {
	this->calculate_projection();
	Instance::update(deltaTime);
}


void Camera::calculate_projection() {
	const auto renderService = Application::instance()->currentModel->get_service<Services::RenderingService>("RenderingService");
	const auto size = renderService->window.getSize();

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