#include "OpenGLVertexBuffer.h"
#include "glad/glad.h"

using namespace Nyanners::Resources::OpenGL;

OpenGLVertexBuffer::OpenGLVertexBuffer() {
	glGenBuffers(1, &bufferId);
	glBindBuffer(GL_ARRAY_BUFFER, bufferId);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() {
	glDeleteBuffers(1, &bufferId);
}

void OpenGLVertexBuffer::use() {
	glBindBuffer(GL_ARRAY_BUFFER, bufferId);
}

void OpenGLVertexBuffer::release() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLVertexBuffer::upload_vertices(const DataTypes::Vertices &vertices) {
	glBufferData(
		GL_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(vertices.size()  * sizeof(float)),
		vertices.data(),
		GL_STATIC_DRAW
	);

	// glEnableVertexAttribArray(0);
	// glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr);
	// glEnableVertexAttribArray(1);
	// glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
}