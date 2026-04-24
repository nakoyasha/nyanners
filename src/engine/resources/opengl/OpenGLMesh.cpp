#include "OpenGLMesh.h"
#include "glad/glad.h"
#include "utils/glCheck.h"

using namespace Nyanners::Resources::OpenGL;

OpenGLMesh::OpenGLMesh() {
	glGenVertexArrays(1, &vertexArrayId);
	glBindVertexArray(vertexArrayId);

	this->vertexBuffer = VertexBuffer::create();
	this->indexBuffer = IndexBuffer::create();

	vertexBuffer->use();
	// glEnableVertexAttribArray(0);
	// glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	vertexBuffer->release();

	glBindVertexArray(0);
}

OpenGLMesh::~OpenGLMesh() {
	delete vertexBuffer;
	delete indexBuffer;
}

void OpenGLMesh::load_from_obj_file(const std::filesystem::path &path) {
	Mesh::load_from_obj_file(path);
}

void OpenGLMesh::set_vertices(const DataTypes::Vertices &newVertices) {
	glBindVertexArray(vertexArrayId);
	vertexBuffer->use();

	GL_CHECK(glBufferData(
		GL_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(newVertices.size()  * sizeof(float)),
		newVertices.data(),
		GL_STATIC_DRAW
	));

	vertexCount = static_cast<int>(newVertices.size());
}

void OpenGLMesh::set_indexes(const std::vector<unsigned> &indexes) {
	indexBuffer->upload_indices(indexes);
	indexCount = indexBuffer->indexCount;
}

void OpenGLMesh::bind() const {
	glBindVertexArray(vertexArrayId);
	vertexBuffer->use();
	indexBuffer->use();
}

void OpenGLMesh::unbind() const {
	glBindVertexArray(0);
	vertexBuffer->release();
	indexBuffer->release();
}