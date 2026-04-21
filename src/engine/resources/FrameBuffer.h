#pragma once
#include "Texture.h"
#include "glad/glad.h"
#include "instances/datatypes/Vector.h"

namespace Nyanners::Resources {
		class FrameBuffer {
		public:
			DataTypes::Vector2 size = DataTypes::Vector2({0, 0});

			FrameBuffer(const int width, const int height);
			~FrameBuffer();
			GLuint get_texture_id();

			void use();
			void release();
			void resize(const int width, const int height);
		private:
			GLuint framebufferId;
			GLuint renderBufferId;
			GLuint depthBufferId;
			std::shared_ptr<Resources::Texture> framebufferTexture;
		};
}
