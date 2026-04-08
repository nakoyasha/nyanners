#pragma once
#include "resources/IndexBuffer.h"
#include "glad/glad.h"

namespace Nyanners::Resources::OpenGL {
	class OpenGLIndexBuffer : public IndexBuffer {
	public:
		OpenGLIndexBuffer();
		~OpenGLIndexBuffer();

		void use() override;
		void release() override;
		void upload_indices(const std::vector<uint32_t> &indices) override;
	private:
		GLuint bufferId = 0;
	};
}