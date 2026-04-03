#pragma once
#include "instances/datatypes/Color3.h"
#include "resources/Shader.h"

#include "SFML/Graphics/RenderTarget.hpp"
#include "third_party/sfml/extlibs/headers/glad/include/glad/gl.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Nyanners::DataTypes {
	using Vertices = std::vector<GLfloat>;
}

namespace Nyanners::Instances {
  class Drawable {
  public:
		glm::vec3 position;
  	glm::mat4 transform;
  	Resources::Shader currentShader;

  	int vertexCount = 0;
  	GLuint vertexBufferId {};
  	GLuint indexBufferId {};

  	int indexCount;
  	GLuint vertexArrayID {};
  	Nyanners::DataTypes::Vertices vertices {};

  	bool isOpaque;

  	DataTypes::Color3 color = {255, 255, 255, 255};

  	Drawable();
		virtual ~Drawable();

  	virtual void set_position(const glm::vec3& newPosition);

		virtual void set_color(const DataTypes::Color3& newColor);

		virtual bool isLegacy();
    virtual void draw() {};
    virtual void draw(const sf::RenderTarget &target) {};
  };
}
