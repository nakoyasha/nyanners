#pragma once
#include <glm/gtc/matrix_transform.hpp>

namespace Nyanners::Instances {
  class Transformable {
  public:
    glm::vec3 position;
    glm::mat4 transform;
    Transformable() : position(1.0f), transform(1.0f) {};

    virtual void set_position(const glm::vec3& newPosition) {
      this->position = newPosition;
      this->transform = glm::translate(this->transform, newPosition);
    }

    virtual ~Transformable() = default;
  };
}
