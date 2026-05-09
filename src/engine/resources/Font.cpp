#include "Font.h"
#include "instances/services/EngineService.h"
#include "utils/glCheck.h"

using namespace Nyanners::Resources;

Font::Font(
  const std::filesystem::path path
) {
	if (FT_Init_FreeType(&ft))
	{
		Services::EngineService::panic("FreeType initialization failed while attempting to load font");
	}

	if (FT_New_Face(ft, path.string().c_str(), 0, &fontFace))
	{
		Core::Logger::log_error(std::format("Failed to load font {}", path.string()));
		return;
	}

	FT_Set_Pixel_Sizes(fontFace, 0, 48);

	for (unsigned char c = 0; c < 128; c++) {
		generate_character(c);
	}
}

void Font::generate_character(const char vChar) {
	if (FT_Load_Char(fontFace, vChar, FT_LOAD_RENDER)) {
		Core::Logger::log(std::format("Could not load character {} from arial.ttf", vChar));
	}

	auto texture = Texture::create(Texture2D);
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

	texture->debugIdentifier = "CharTexture";
	texture->set_texture_parameter(TextureWrapCoordinateT, ClampToEdge);
	texture->set_texture_parameter(TextureWrapCoordinateS, ClampToEdge);
	texture->set_texture_parameter(MinificationFilter, Linear);
	texture->set_texture_parameter(MagnificationFilter, Linear);

	DataTypes::Character character = {
		texture,
		glm::ivec2(fontFace->glyph->bitmap.width, fontFace->glyph->bitmap.rows),
		glm::ivec2(fontFace->glyph->bitmap_left, fontFace->glyph->bitmap_top),
		fontFace->glyph->advance.x
};
	characters.insert(std::pair<char, DataTypes::Character>(vChar, character));
	texture->unuse();
}