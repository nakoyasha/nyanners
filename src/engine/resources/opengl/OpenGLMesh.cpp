#include "OpenGLMesh.h"
#include "glad/glad.h"

using namespace Nyanners::Resources::OpenGL;

OpenGLMesh::OpenGLMesh() {
	this->vertexBuffer = VertexBuffer::create();
	this->indexBuffer = IndexBuffer::create();
}

OpenGLMesh::~OpenGLMesh() {
	delete vertexBuffer;
	delete indexBuffer;
}

void OpenGLMesh::load_from_obj_file(const std::filesystem::path &path) {
	Mesh::load_from_obj_file(path);
}

void OpenGLMesh::set_vertices(const DataTypes::Vertices &newVertices) {
	vertexBuffer->use();

	glBufferData(
		GL_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(newVertices.size()  * sizeof(float)),
		newVertices.data(),
		GL_STATIC_DRAW
	);

	vertexCount = static_cast<int>(newVertices.size());
	vertexBuffer->release();
}

void OpenGLMesh::set_indexes(const std::vector<unsigned> &indexes) {
	indexBuffer->use();

	indexBuffer->upload_indices(indexes);
	indexCount = indexBuffer->indexCount;

	indexBuffer->release();
}

void OpenGLMesh::bind() const {
	vertexBuffer->use();
	indexBuffer->use();
}

void OpenGLMesh::unbind() const {
	vertexBuffer->release();
	indexBuffer->release();
}