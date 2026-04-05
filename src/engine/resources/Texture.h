#pragma once
#include "SFML/Graphics/Image.hpp"
#include "third_party/sfml/extlibs/headers/glad/include/glad/gl.h"
#include <filesystem>

namespace Nyanners::Resources {
	class Texture {
	public:
		Texture();
		Texture(const std::filesystem::path& path);
		~Texture();

		int height;
		int width;
		GLuint textureId = 0;

		void  *textureBuffer;

		void load_from_file(const std::filesystem::path& path);
		void upload_buffer(
		  int internalFormat,
		  int externalFormat,
		  int width,
		  int height,
		  const void *imageBuffer
		) const;
		void set_mipmap_enabled(const bool newState);
		void use();
		void unuse();
	private:

		bool useMipmaps = false;
		bool inUse = false;
		int channelsInFile = 4;
	};
}