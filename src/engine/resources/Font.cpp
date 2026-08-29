#include "Font.h"
#include "Application.h"
#include "instances/services/EngineService.h"
#include "instances/services/RenderingService.h"
#include "utils/glCheck.h"

using namespace Nyanners::Resources;

Font::Font(const std::filesystem::path path) {
	if (FT_Init_FreeType(&ft))
	{
		Services::EngineService::panic("FreeType initialization failed while attempting to load font");
	}

	if (FT_New_Face(ft, path.string().c_str(), 0, &fontFace))
	{
		Core::Logger::log_error(std::format("Failed to load font {}", path.string()));
		return;
	}

	this->set_font_size(fontHeight);

	for (unsigned char c = 0; c < 128; c++) {
		generate_character(c);
	}
}

void Font::set_font_size(int newSize) {
	FT_Set_Pixel_Sizes(fontFace, 0, newSize);
	fontHeight = newSize;

	// texture won't get nuked by itself, unfortunately :v
	for (const auto& [_, character] : characters) {
		Services::RenderingService::remove_texture(character.texture);
	}

	// regenerate characters at the new size
	characters.clear();
	for (unsigned char c = 0; c < 128; c++) {
		generate_character(c);
	}
}

void Font::generate_character(const char vChar) {
	if (FT_Load_Char(fontFace, vChar, FT_LOAD_RENDER)) {
		Core::Logger::log(std::format("Could not load character {} from arial.ttf", vChar));
	}

	DataTypes::Character character = {
		nullptr,
		glm::ivec2(fontFace->glyph->bitmap.width, fontFace->glyph->bitmap.rows),
		glm::ivec2(fontFace->glyph->bitmap_left, fontFace->glyph->bitmap_top),
		fontFace->glyph->advance.x
	};

	// skip for special characters
	if (!std::isspace(static_cast<unsigned char>(vChar)) && vChar != '\0') {
		const auto texture = Texture::create(Texture2D);
		// TODO: abstract this:
		GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
		texture->use();
		texture->upload_buffer(
				GL_RED,
				GL_RED,
				fontFace->glyph->bitmap.width,
				fontFace->glyph->bitmap.rows,
				fontFace->glyph->bitmap.buffer
				);

		texture->debugIdentifier = std::format("CharTexture-{}", vChar);
		texture->set_texture_parameter(TextureWrapCoordinateT, ClampToEdge);
		texture->set_texture_parameter(TextureWrapCoordinateS, ClampToEdge);
		texture->set_texture_parameter(MinificationFilter, Linear);
		texture->set_texture_parameter(MagnificationFilter, Linear);

		character.texture = texture;
		texture->unuse();
	} else {
		Core::Logger::log_debug("Skipped empty character");
	}


	characters.insert(std::pair(vChar, character));
}
