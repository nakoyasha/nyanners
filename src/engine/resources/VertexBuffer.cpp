#include "VertexBuffer.h"
#include "opengl/OpenGLVertexBuffer.h"

using namespace Nyanners::Resources;

VertexBuffer* VertexBuffer::create() {
	return new OpenGL::OpenGLVertexBuffer();
}