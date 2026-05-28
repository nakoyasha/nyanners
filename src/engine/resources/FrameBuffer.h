#pragma once
#include "Texture.h"
#include "glad/glad.h"
#include "instances/datatypes/Vector.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Nyanners::Resources {
		class FrameBuffer {
		public:
			std::shared_ptr<Resources::Texture> framebufferTexture;
			glm::vec2 size {};

			FrameBuffer(const int width, const int height);
			~FrameBuffer();
			GLuint get_texture_id();

			void use() const;
			void release() const;
			void clear();
			void resize(const int width, const int height);
		private:
			GLuint framebufferId;
			GLuint renderBufferId;
			GLuint depthBufferId;

			void construct_framebuffer(const int width, const int height);
			void check_status();
		};
}
