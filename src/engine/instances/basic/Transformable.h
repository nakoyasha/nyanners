#pragma once
#include <glm/gtc/matrix_transform.hpp>

namespace Nyanners::Instances {
	class Transformable {
	public:
		virtual ~Transformable() = default;

		glm::vec3* position;
		glm::mat4 transform;
		glm::vec3 scale {1.0f, 1.0f, 1.0f};

		Transformable() {
			position = new glm::vec3();
		}

		virtual void set_position(const glm::vec3& newPosition) {
			this->position = new glm::vec3(newPosition);
			this->transform = glm::translate(glm::mat4(1.0f), newPosition);
		};

		virtual void set_scale(const glm::vec3& newScale) {
			this->transform = glm::scale(transform, newScale);
		};
	};
}