#pragma once

#include "instances/datatypes/Color3.h"
#include "resources/Material.h"
#include "resources/Mesh.h"
#include "glad/glad.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Nyanners::Instances {
  class Drawable {
  public:
		glm::vec3* position;
  	glm::mat4 transform;
  	glm::vec3 scale {1.0f, 1.0f, 1.0f};

  	Resources::Material* material;
  	Resources::Mesh* mesh;

  	int indexCount;
  	GLuint vertexArrayID {};

  	bool isOpaque = true;

		Drawable();
		virtual ~Drawable();

  	virtual void set_position(const glm::vec3& newPosition);
  	virtual void set_scale(const glm::vec3& newScale);
  	virtual void set_color(DataTypes::Color3 newColor);
  	virtual void set_color(DataTypes::Color3* newColor);

  	virtual bool isLegacy();
    virtual void draw() {};
  };
}
