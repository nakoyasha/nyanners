#pragma once
#include "instances/Instance.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Nyanners::Instances {
	class Camera : public Instance {
	public:
		Camera();
		unsigned int fov = 80;

		glm::mat4 view = glm::lookAt(
		  glm::vec3(4, 5, 3), // Camera is at (4,3,3), in World Space
		  glm::vec3(0, 0, 0), // and looks at the origin
		  glm::vec3(0, 1, 0) // Head is up (set to 0,-1,0 to look upside-down)
		);

		void update(const float deltaTime) override;

		glm::mat4 projection;
	private:
		void calculate_projection();
		// Camera state
		glm::vec3 cameraPos   = { 0.0f, 0.0f, 3.0f };
		glm::vec3 cameraFront = { 0.0f, 0.0f, -1.0f };
		glm::vec3 cameraUp    = { 0.0f, 1.0f, 0.0f };

		float yaw   = -90.0f; // look forward
		float pitch = 0.0f;

		float moveSpeed = 6.0f;
		float mouseSens = 0.1f;

		bool firstMouse = true;
		sf::Vector2i lastMouse;
	};
}