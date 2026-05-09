#include <ft2build.h>
#include FT_FREETYPE_H

#include "TextLabel.h"
#include "instances/services/RenderingService.h"
#include "utils/glCheck.h"

using namespace Nyanners::Instances;

namespace Nyanners::Instances {
	const ReflectionClass& instance = Services::ReflectionService::create_reflection(
		{
			.className = "TextLabel",
			.base = "Drawable",
			.flags = {Scripting::Reflection::ReflectionInstanceFlags::Creatable},
			.constructor = &Scripting::Reflection::create_instance<TextLabel>,
			.properties = {
			 {
			 	.name = "Text",
				.type = String,
				.get =
					[](const Instance *instance, lua_State *context) {
						const auto *label =
						dynamic_cast<const Instances::TextLabel *>(instance);

						lua_pushstring(context, label->text.c_str());

						return 1;
					},
				.set =
					[](Instance *instance, lua_State *context) {
						auto *label = dynamic_cast<Instances::TextLabel *>(instance);
						const std::string text = luaL_checkstring(context, -1);

						label->set_text(text);
					}},
					 {.name = "MaxVisibleGlyphs",
		 .type = ReflectionPropertyType::Number,
		 .get =
		 [](const Instance *instance, lua_State *context) {
			 const auto *label =
			 dynamic_cast<const Instances::TextLabel *>(instance);

			 lua_pushnumber(context, label->maxVisibleGlyph);

			 return 1;
		 },
		 .set =
		 [](Instance *instance, lua_State *context) {
			 auto *label = dynamic_cast<Instances::TextLabel *>(instance);
			 const int newGlyphs = luaL_checknumber(context, -1);

			 label->maxVisibleGlyph = newGlyphs;
		 }}
		 },
		}
	);
}

TextLabel::TextLabel() : Instance("TextLabel") {
		font = new Resources::Font("assets/fonts/arial.ttf");

    material->shader->load_from_file("assets/shaders/textVert.glsl", "assets/shaders/textFrag.glsl");
    material->shader->use();
    material->shader->setInt("uTexture", 0);

    mesh = Resources::Mesh::create();
    mesh->bind();

		mesh->bind();
		mesh->set_vertices({
			0.f, 1.f,  0.f, 0.f,
			0.f, 0.f,  0.f, 1.f,
			1.f, 0.f,  1.f, 1.f,
			1.f, 1.f,  1.f, 0.f
		});

		// mesh->set_indexes({0, 1, 2, 0, 3, 4});
		mesh->set_indexes({0, 1, 2, 0, 2, 3});
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    Drawable::set_color({255, 255, 255, 255});
    mesh->unbind();
}

TextLabel::~TextLabel() {
	glyphs.clear();
	delete font;
}

void TextLabel::draw() {
		// make sure the label can render
		Services::RenderingService::renderer->set_depth_test(Core::Rendering::DepthCheckLevel::Always);
		Services::RenderingService::renderer->disable_depth_buffer();
		Services::RenderingService::renderer->set_renderer_feature(Core::Rendering::RendererFeature::FaceCulling, false);

    // NOTE: this is all mostly taken from learn opengl
    this->material->use();

		this->material->shader->setMatrix("uProjection", Services::RenderingService::renderer->projection2D);
    int renderedGlyph = 0;

		for (const auto& glyph : glyphs) {
			const auto xPosition = glyph.position.x;
			const auto yPosition = glyph.position.y;
			const auto character = glyph.character;

			const auto glyphWidth = glyph.size.x;
			const auto glyphHeight = glyph.size.y;
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
			mesh->bind();
			Services::RenderingService::renderer->render_mesh(mesh);

			// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			// globalPositionX += (character.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
			renderedGlyph += 1;
		}

		// reset
		Services::RenderingService::renderer->enable_depth_buffer();
		Services::RenderingService::renderer->set_previous_depth_test();
	Services::RenderingService::renderer->set_renderer_feature(Core::Rendering::RendererFeature::FaceCulling, true);
}

void TextLabel::update(const float deltaTime) {
    Instance::update(deltaTime);
    uiPosition.recomputeSize();
}

void TextLabel::set_text(const std::string &newText) {
    text = newText;
		glyphs.clear();
		calculate_text(this->text);
}

void TextLabel::set_position(const glm::vec3 &newPosition) {
    uiPosition.setX(newPosition.x);
    uiPosition.setY(newPosition.y);

    Drawable::set_position(newPosition);

		// recalculate as they'd have to be at a new position now
		glyphs.clear();
		calculate_text(this->text);
}

void TextLabel::calculate_text(const std::string &text) {
	float globalPositionX = position->x;
	float globalPositionY = position->y;

	int renderedGlyph = 0;

	for (std::string::const_iterator iterator = text.begin(); iterator != text.end(); ++iterator) {
		if (renderedGlyph >= maxVisibleGlyph && maxVisibleGlyph != -1) {
			break;
		}

		const auto result = font->characters.find(*iterator);

		if (result == font->characters.end()) {
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

		DataTypes::CalculatedGlyph glyph {};
		glyph.position = glm::vec2(xPosition, yPosition);
		glyph.size = glm::vec2(glyphWidth, glyphHeight);
		glyph.character = character;

		glyphs.push_back(glyph);
		globalPositionX += (character.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	};
}
