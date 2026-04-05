#include "Drawable.h"
#include "resources/Mesh.h"
#include "instances/services/RenderingService.h"

using namespace Nyanners::Instances;

Drawable::Drawable() : position(1.0f), transform(1.0f), indexCount(0) {
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	mesh = Resources::Mesh::create();

	currentShader = Services::RenderingService::defaultShader;
	currentShader.use();

	Drawable::set_color(color);

	glBindVertexArray(0);
	glUseProgram(0);
}

Drawable::~Drawable() {
	delete mesh;
	glDeleteBuffers(1, &vertexArrayID);
}

void Drawable::set_position(const glm::vec3 &newPosition) {
	this->position = newPosition;
	this->transform = glm::translate(glm::mat4(1.0f), newPosition);
}

void Drawable::set_color(const DataTypes::Color3 &newColor) {
	glBindVertexArray(vertexArrayID);

	this->color = newColor;
	this->currentShader.use();
	this->currentShader.setColor("uColor", newColor);

	glBindVertexArray(0);
}

bool Drawable::isLegacy() {
	return false;
}
