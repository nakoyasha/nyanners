#pragma once
#include <glm/gtc/matrix_transform.hpp>

namespace Nyanners::Instances {
  class Transformable {
  public:
    glm::mat4 transform;
    Transformable() : transform(1.0f) {};
    virtual ~Transformable() = default;
  };
}
