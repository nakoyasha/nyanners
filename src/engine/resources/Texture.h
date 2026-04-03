#pragma once
#include "third_party/sfml/extlibs/headers/glad/include/glad/gl.h"
#include <filesystem>

namespace Nyanners::Resources {
	class Texture {
	public:
		Texture();
		~Texture();

		int height;
		int width;

		unsigned char* textureBuffer;

		void load_from_file(const std::filesystem::path& path);
		void set_mipmap_enabled(const bool newState);
		void use();
		void unuse();
	private:
		GLuint textureId = 0;

		bool useMipmaps = false;
		bool inUse = false;
		int channelsInFile = 4;
	};
}