#pragma once

#include "glad/glad.h"
#include "instances/basic/Transformable.h"
#include "instances/datatypes/Color3.h"
#include "resources/Material.h"
#include "resources/Mesh.h"

namespace Nyanners::Instances {
  class Drawable : public Transformable {
  public:
  	Resources::Material* material;
  	Resources::Mesh* mesh;

  	int indexCount;
  	GLuint vertexArrayID {};

  	bool isOpaque = true;

		Drawable();
		virtual ~Drawable();

  	virtual void set_color(DataTypes::Color3 newColor);
  	virtual void set_color(DataTypes::Color3* newColor);

  	virtual bool isLegacy();
    virtual void draw() {};
  };
}
