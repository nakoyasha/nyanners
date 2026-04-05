#pragma once
#include "third_party/sfml/extlibs/headers/glad/include/glad/gl.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Nyanners::DataTypes {
	using Vertices = std::vector<GLfloat>;
}

namespace Nyanners::Instances {
  class WorldObject {
  public:
		glm::vec3 position;
  	glm::mat4 transform;
  	bool isOpaque;

		virtual ~WorldObject() = default;
  	virtual void set_position(const glm::vec3& newPosition);
  };
}
