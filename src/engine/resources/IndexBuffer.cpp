#include "IndexBuffer.h"
#include "opengl/OpenGLIndexBuffer.h"

using namespace Nyanners::Resources;

IndexBuffer *IndexBuffer::create() {
	return new OpenGL::OpenGLIndexBuffer();
}