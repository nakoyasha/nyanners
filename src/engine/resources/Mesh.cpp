#include "Mesh.h"
#include "opengl/OpenGLMesh.h"

using namespace Nyanners::Resources;

void Mesh::load_from_obj_file(const std::filesystem::path &path) {
	throw std::runtime_error("Unimplemented");
}

Mesh *Mesh::create() {
	return new OpenGL::OpenGLMesh();
}