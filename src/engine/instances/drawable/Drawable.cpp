#include "Drawable.h"
#include "resources/Mesh.h"
#include "instances/services/RenderingService.h"

using namespace Nyanners::Instances;

Drawable::Drawable() : transform(1.0f), indexCount(0) {
	position = new glm::vec3();
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

void Drawable::set_position(const glm::vec3 &newPosition) {
	this->position = new glm::vec3(newPosition);
	this->transform = glm::translate(glm::mat4(1.0f), newPosition);
}

void Drawable::set_scale(const glm::vec3 &newScale) {
	this->transform = glm::scale(transform, newScale);
}

void Drawable::set_color(const DataTypes::Color3 newColor) {
	glBindVertexArray(vertexArrayID);

	this->material->set_color(new DataTypes::Color3(newColor));

	glBindVertexArray(0);
}

void Drawable::set_color(DataTypes::Color3 *newColor) {
	glBindVertexArray(vertexArrayID);

	this->material->set_color(newColor);

	glBindVertexArray(0);
}

bool Drawable::isLegacy() {
	return false;
}
