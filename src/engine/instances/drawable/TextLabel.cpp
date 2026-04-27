#include <ft2build.h>
#include FT_FREETYPE_H

#include "TextLabel.h"
#include "instances/services/RenderingService.h"
#include "utils/glCheck.h"

using namespace Nyanners::Instances;

TextLabel::TextLabel() : Instance("TextLabel") {
    if (FT_Init_FreeType(&ft))
    {
        Core::Logger::log_error("Could not initialize FreeType. oh well");
        return;
    }

    if (FT_New_Face(ft, "assets/fonts/arial.ttf", 0, &fontFace))
    {
        Core::Logger::log_error("Failed to load default arial");
        return;
    }

    FT_Set_Pixel_Sizes(fontFace, 0, 48);
    material->shader->load_from_file("assets/shaders/textVert.glsl", "assets/shaders/textFrag.glsl");
    material->shader->use();
    material->shader->setInt("uTexture", 0);

    mesh = Resources::Mesh::create();
    mesh->bind();

    for (unsigned char c = 0; c < 128; c++) {
        generate_character(c);
    }

    Drawable::set_color({255, 255, 255, 255});
    mesh->unbind();
}

void TextLabel::draw() {
		// make sure the label can render
		Services::RenderingService::renderer->set_depth_test(Core::Rendering::DepthCheckLevel::Always);
		Services::RenderingService::renderer->disable_depth_buffer();

    // NOTE: this is all mostly taken from learn opengl
    this->material->use();

		this->material->shader->setMatrix("uProjection", Services::RenderingService::renderer->projection2D);
		this->material->shader->setBool("uUseScreenSpace", !useWorldSpace);

    float globalPositionX = position->x;
    float globalPositionY = position->y;

    int renderedGlyph = 0;

    for (std::string::const_iterator iterator = text.begin(); iterator != text.end(); ++iterator)
    {
        if (renderedGlyph >= maxVisibleGlyph && maxVisibleGlyph != -1) {
            break;
        }

        const auto result = characters.find(*iterator);

        if (result == characters.end()) {
            Core::Logger::log(std::format("ERROR: Unknown character {}, will not render!!", *iterator));
            continue;
        }

        const auto character = result->second;

        // taken from https://youtu.be/S0PyZKX4lyI, very good watch
        if (*iterator == '\n') {
            globalPositionY -= (character.size.y * lineHeight) * scale;
            globalPositionX = position->x;
            continue;
        }

        if (*iterator == ' ') {
            globalPositionX += (character.advance >> 6) * scale;
            continue;
        }

        float xPosition = globalPositionX + character.bearing.x * scale;
        float yPosition = globalPositionY - (character.size.y - character.bearing.y) * scale;

        const float glyphWidth = character.size.x * scale;
        const float glyphHeight = character.size.y * scale;

        // render glyph texture over quad
        this->material->set_texture(character.texture);

        mesh->bind();
        mesh->set_vertices({
            xPosition,              yPosition + glyphHeight,    0.0f, 0.0f,
            xPosition,              yPosition,                  0.0f, 1.0f,
            xPosition + glyphWidth, yPosition,                  1.0f, 1.0f,

            // xPosition,              yPosition + glyphHeight,    0.0f, 0.0f,
            xPosition + glyphWidth, yPosition,                  1.0f, 1.0f,
            xPosition + glyphWidth, yPosition + glyphHeight,    1.0f, 0.0f
        });

        mesh->set_indexes({0, 1, 2, 0, 3, 4});

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    		this->material->use();
        Services::RenderingService::renderer->render_mesh(mesh);

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        globalPositionX += (character.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
        renderedGlyph += 1;
    }

    this->material->release();

		// reset
		Services::RenderingService::renderer->enable_depth_buffer();
		Services::RenderingService::renderer->set_previous_depth_test();
}

void TextLabel::update(const float deltaTime) {
    Instance::update(deltaTime);
    uiPosition.recomputeSize();
}

void TextLabel::set_text(const std::string &newText) {
    text = newText;
}

void TextLabel::set_position(const glm::vec3 &newPosition) {
    uiPosition.setX(newPosition.x);
    uiPosition.setY(newPosition.y);

    // Drawable::set_position({uiPosition.absoluteX, uiPosition.absoluteY, 0.0f});
    Drawable::set_position(newPosition);
}

void TextLabel::generate_character(unsigned char vChar) {
    if (FT_Load_Char(fontFace, vChar, FT_LOAD_RENDER)) {
        Core::Logger::log(std::format("Could not load character {} from arial.ttf", vChar));
    }

    auto texture = Resources::Texture::create(Texture2D);
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
