#pragma once
#include "Texture.h"
#include "glad/glad.h"

namespace Nyanners::Resources {
		class FrameBuffer {
		public:
			FrameBuffer(const int width, const int height);
			~FrameBuffer();
			GLuint get_texture_id();

			void use();
			void release();
			void resize(const int width, const int height);
		private:
			GLuint framebufferId;
			GLuint renderBufferId;
			Resources::Texture* framebufferTexture;
		};
}