#pragma once
#include <glm/gtc/matrix_transform.hpp>

namespace Nyanners::Instances {
	class Transformable {
	public:
		virtual ~Transformable() = default;

		glm::vec3* position;
		glm::vec3* rotation;
		glm::mat4 transform {};
		glm::vec3* scale;

		Transformable() {
			position = new glm::vec3();
			rotation = new glm::vec3();
			scale = new glm::vec3({1.0f, 1.0f, 1.0f});
		}

		glm::mat4 get_transform() {
			this->transform = glm::scale(glm::translate(glm::mat4(1.0f), *this->position), *this->scale);

			// x
			this->transform = glm::rotate(transform, glm::radians(rotation->x), glm::vec3(1.0f, 0.0f, 0.0f));
			// y
			this->transform = glm::rotate(transform, glm::radians(rotation->y), glm::vec3(0.0f, 1.0f, 0.0f));
			// z
			this->transform = glm::rotate(transform, glm::radians(rotation->z), glm::vec3(0.0f, 0.0f, 1.0f));

			return this->transform;
		}

		[[nodiscard]] virtual glm::vec3 get_position() const {
			return *position;
		};

		[[nodiscard]] virtual glm::vec3 get_rotation() const {
			return *rotation;
		};

		[[nodiscard]] virtual glm::vec3 get_scale() const {
			return *scale;
		};

		virtual void set_position(const glm::vec3& newPosition) {
			position = new glm::vec3(newPosition);
		};

		virtual void set_scale(const glm::vec3& newScale) {
			scale = new glm::vec3(newScale);
		};

		virtual void set_rotation(const glm::vec3& newRotation) {
			// // x
			// this->transform = glm::rotate(transform, glm::radians(newRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			// // y
			// this->transform = glm::rotate(transform, glm::radians(newRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			// // z
			// this->transform = glm::rotate(transform, glm::radians(newRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			rotation->x = newRotation.x;
			rotation->y = newRotation.y;
			rotation->z = newRotation.z;
		};
	};
}