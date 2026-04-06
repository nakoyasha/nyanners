#include "Material.h"
#include "opengl/OpenGLMaterial.h"

using namespace Nyanners::Resources;
Material *Material::create() {
	return new OpenGL::OpenGLMaterial();
}