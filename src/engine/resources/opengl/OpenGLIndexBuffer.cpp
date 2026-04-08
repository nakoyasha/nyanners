#include "OpenGLIndexBuffer.h"
#include "glad/glad.h"

using namespace Nyanners::Resources::OpenGL;

OpenGLIndexBuffer::OpenGLIndexBuffer() {
	glGenBuffers(1, &bufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer() {
	glDeleteBuffers(1, &bufferId);
}

void OpenGLIndexBuffer::use() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
}

void OpenGLIndexBuffer::release() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void OpenGLIndexBuffer::upload_indices(const std::vector<uint32_t> &indices) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);

	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)),
		indices.data(),
	GL_STATIC_DRAW
	);

	indexCount = static_cast<int>(indices.size());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}