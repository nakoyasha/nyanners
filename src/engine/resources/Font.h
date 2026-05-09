#pragma once
#include "Texture.h"
#include "freetype/freetype.h"
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include <memory>
#include <string>


namespace Nyanners::DataTypes {
	struct Character {
		std::shared_ptr<Resources::Texture> texture;
		glm::ivec2 size;
		glm::ivec2 bearing;
		long advance; // next glyph offset
	};
}

namespace Nyanners::Resources {
	class Font {
		public:

		int fontHeight = 48;
		int fontWidth = 0;

		const std::string fontPath {};
		std::map<char, DataTypes::Character> characters;

		Font(const std::filesystem::path path);
		~Font() {
			FT_Done_Face(fontFace);
			FT_Done_FreeType(ft);
			characters.clear();
		};

	private:
		FT_Library ft;
		FT_Face fontFace;
		void generate_character(const char vChar);
	};
}