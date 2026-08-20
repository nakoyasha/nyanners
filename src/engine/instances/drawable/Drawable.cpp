#include "Drawable.h"

#include "lualib.h"
#include "resources/Mesh.h"
#include "instances/services/RenderingService.h"

using namespace Nyanners::Instances;

namespace Nyanners::Scripting {
	static auto drawableDescriptor = ReflectionDescriptorRegistry::instance()->create_registrator([]() {
		Services::ReflectionService::create_descriptor("Drawable", {})
		.add_property_chained<Drawable, DataTypes::Color3, &Drawable::get_color, &Drawable::set_color>("Color", Color)
		.add_method<&Drawable::lua_set_texture>("set_texture", Null, {{"textureFile", String}});
	});
}

Drawable::Drawable() : Transformable(), indexCount(0) {
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	mesh = Resources::Mesh::create();
	material = Resources::Material::create();

	Drawable::set_color({255, 255, 255, 255});

	glBindVertexArray(0);
	glUseProgram(0);
}

Drawable::~Drawable() {
	delete mesh;
	glDeleteBuffers(1, &vertexArrayID);
}

Nyanners::DataTypes::Color3 Drawable::get_color() const {
	return this->material->color;
}

void Drawable::set_color(const DataTypes::Color3 newColor) {
	glBindVertexArray(vertexArrayID);

	this->material->set_color(newColor);

	glBindVertexArray(0);
}

void Drawable::lua_set_texture(std::string path) {
	this->material->set_texture(path);
}
