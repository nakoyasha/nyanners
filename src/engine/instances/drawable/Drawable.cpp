#include "Drawable.h"
#include "instances/services/RenderingService.h"

using namespace Nyanners::Instances;

Drawable::Drawable() : position(1.0f), transform(1.0f), indexCount(0) {
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);

	// OpenGL calls this "GL_ELEMENT_ARRAY_BUFFER", but the more sensible
	// name for it is an index buffer.

	// This buffer holds a list of "indices" (aka indexes), which
	// correspond to said indexes in the vertexBuffer.
	// So, to render vertice 1 5 times, we'd have the number the buffer as:
	// [0, 0, 0, 0, 0]

	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);

	currentShader = Services::RenderingService::defaultShader;
	currentShader.use();

	Drawable::set_color(color);

	glBindVertexArray(0);
	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Drawable::~Drawable() {
	glDeleteBuffers(1, &vertexBufferId);
	glDeleteBuffers(1, &vertexArrayID);
	glDeleteBuffers(1, &indexBufferId);
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
