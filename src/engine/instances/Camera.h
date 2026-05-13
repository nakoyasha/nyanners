#pragma once

#include "instances/Instance.h"
#include <glm/gtc/matrix_transform.hpp>

#include "datatypes/Vector.h"

namespace Nyanners::Instances {
	class Camera : public Instance, public Transformable {
	public:
		Camera();

		DataTypes::Vector2* resolution;
		glm::mat4 projection {};
		glm::mat4 view = glm::lookAt(
		  glm::vec3(4, 5, 3), // Camera is at (4,3,3), in World Space
		  glm::vec3(0, 0, 0), // and looks at the origin
		  glm::vec3(0, 1, 0) // Head is up (set to 0,-1,0 to look upside-down)
		);

		unsigned int fov = 80;
		bool useDebugMovement = true;

		void update(const float deltaTime) override;
		void set_fov(unsigned int newFov);
		void calculate_projection(const DataTypes::Vector2 &size, bool forceRecalculate);
		void set_position(const glm::vec3 &newPosition) override;

	private:
		DataTypes::Vector2 lastSize {0,0};
		// Camera state
		glm::vec3 cameraFront = { 0.0f, 0.0f, -1.0f };
		glm::vec3 cameraUp    = { 0.0f, 1.0f, 0.0f };

		float yaw   = -90.0f; // look forward
		float pitch = 0.0f;

		float moveSpeed = 6.0f;
		float mouseSens = 0.1f;
	};
}
