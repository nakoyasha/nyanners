#pragma once
#include "SFML/Graphics/RenderTarget.hpp"
#include "instances/datatypes/Color3.h"
#include "resources/Material.h"
#include "resources/Mesh.h"
#include "third_party/sfml/extlibs/headers/glad/include/glad/gl.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Nyanners::Instances {
  class Drawable {
  public:
		glm::vec3 position;
  	glm::mat4 transform;
  	Resources::Material* material;
  	Resources::Mesh* mesh;

  	int indexCount;
  	GLuint vertexArrayID {};

  	bool isOpaque = true;

  	DataTypes::Color3 color = {255, 255, 255, 255};
		Drawable();
		virtual ~Drawable();

  	virtual void set_position(const glm::vec3& newPosition);

		virtual void set_color(const DataTypes::Color3& newColor);

		virtual bool isLegacy();
    virtual void draw() {};
    // virtual void draw(const sf::RenderTarget &target) {};
  };
}
