#pragma once
#include "instances/Instance.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Nyanners::Instances {
	class Camera : public Instance {
	public:
		Camera();
		glm::mat4 view = glm::lookAt(
		  glm::vec3(4, 5, 3), // Camera is at (4,3,3), in World Space
		  glm::vec3(0, 0, 0), // and looks at the origin
		  glm::vec3(0, 1, 0) // Head is up (set to 0,-1,0 to look upside-down)
		);

		void update(const float deltaTime) override;

		glm::mat4 projection;
	private:
		void calculate_projection();
	};
}