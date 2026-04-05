#pragma once
#include "resources/VertexBuffer.h"
#include "third_party/sfml/extlibs/headers/glad/include/glad/gl.h"

namespace Nyanners::Resources::OpenGL {
	class OpenGLVertexBuffer : public VertexBuffer {
	public:
		OpenGLVertexBuffer();
		~OpenGLVertexBuffer();

		void use() override;
		void release() override;
		void upload_vertices(const DataTypes::Vertices &vertices) override;
	private:
		GLuint bufferId = 0;
	};
}