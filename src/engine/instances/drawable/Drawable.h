#pragma once

#include "lua.h"
#include "glad/glad.h"
#include "instances/basic/Transformable.h"
#include "instances/datatypes/Color3.h"
#include "resources/Material.h"
#include "resources/Mesh.h"

namespace Nyanners::Instances {
  class Drawable : public Transformable {
  public:
  	int indexCount = 0;
  	bool isOpaque = true;
  	GLuint vertexArrayID {};

  	Resources::Mesh* mesh;
  	Ref<Resources::Material> material;

  	Drawable();
  	~Drawable() override;

  	DataTypes::Color3 get_color() const;
  	virtual void set_color(DataTypes::Color3 newColor);
    void lua_set_texture(std::string path);

    Ref<Resources::Material> get_material() const;
  	void set_material(const Ref<Resources::Material> &newMaterial);

    virtual void draw() = 0;
  };
}
