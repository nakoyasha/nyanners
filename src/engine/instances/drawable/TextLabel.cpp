#include <ft2build.h>
#include FT_FREETYPE_H

#include "TextLabel.h"
#include "instances/services/RenderingService.h"
#include "utils/glCheck.h"

using namespace Nyanners::Instances;

namespace Nyanners::Instances {
	auto registrator = ReflectionDescriptorRegistry::instance()->create_registrator([]() {
		Services::ReflectionService::create_descriptor("TextLabel", {"Instance", "Transformable"})
			.add_property_chained<TextLabel, bool, &TextLabel::get_draw_shadow, &TextLabel::set_draw_shadow>("DrawShadow", Boolean)
			.add_property_chained<TextLabel, std::string, &TextLabel::get_text, &TextLabel::set_text>("Text", String)
			.add_property_chained<TextLabel, DataTypes::Color3, &TextLabel::get_color, &TextLabel::set_color>("Color", Color)
			.add_property_chained<TextLabel, DataTypes::Color3, &TextLabel::get_shadow_color, &TextLabel::set_shadow_color>("ShadowColor", Color);
	});
}

TextLabel::TextLabel() : Instance("TextLabel") {
	font = new Resources::Font("assets/fonts/arial.ttf");
	shadowMaterial = Resources::Material::create();

	material->shader->load_from_file(
	  "assets/shaders/textVert.glsl", "assets/shaders/textFrag.glsl"
	);
	material->shader->use();
	material->shader->setInt("uTexture", 0);

	shadowMaterial->shader->load_from_file(
		"assets/shaders/textVert.glsl", "assets/shaders/textFrag.glsl"
	);
	shadowMaterial->shader->use();
	material->shader->setInt("uTexture", 0);
	shadowMaterial->set_color(shadowColor);
	shadowMaterial->release();

	mesh = Resources::Mesh::create();
	mesh->bind();
	mesh->set_vertices({
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f,0.0f,0.0f,1.0f,
		1.0f,0.0f,1.0f,1.0f,

		// 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f,0.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,0.0f
	});

	mesh->set_indexes({0, 1, 2, 0, 3, 4});
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
	Drawable::set_color({255, 255, 255, 255});
	mesh->unbind();
}

TextLabel::~TextLabel() {
	glyphs.clear();
	delete font;
}

void TextLabel::draw() {
	int renderedGlyph = 0;
	const auto window_size =
	  Services::RenderingService::renderer->get_window_size();

	// make sure the label can render
	Services::RenderingService::renderer->set_depth_test(
	  Core::Rendering::DepthCheckLevel::Always
	);
	Services::RenderingService::renderer->disable_depth_buffer();
	Services::RenderingService::renderer->set_renderer_feature(
	  Core::Rendering::RendererFeature::FaceCulling, false
	);

	// NOTE: this is all mostly taken from learn opengl
	this->material->use();
	this->material->shader->setMatrix(
	  "uProjection", Services::RenderingService::renderer->projection2D
	);
	this->material->release();

	this->shadowMaterial->use();
	this->material->shader->setMatrix(
		"uProjection", Services::RenderingService::renderer->projection2D
	);
	this->shadowMaterial->release();
	this->material->use();

	for (const auto &glyph : glyphs) {
		if (maxVisibleGlyph != -1 && renderedGlyph >= maxVisibleGlyph) {
			break;
		}

		const auto xPosition = glyph.position.x * scale->x;
		const auto yPosition = glyph.position.y * scale->y;
		const auto character = glyph.character;

		if (xPosition > window_size.x) {
			continue;
		}

		if (yPosition > window_size.y) {
			continue;
		}

		const auto glyphWidth = glyph.size.x;
		const auto glyphHeight = glyph.size.y;
		this->material->set_texture(character.texture);

		mesh->bind();
		glm::mat4 transform =
		  glm::translate(glm::mat4(1.0f), glm::vec3(xPosition, yPosition, 0.0f));
		transform = glm::scale(
		  transform, glm::vec3(glyphWidth * scale->x, glyphHeight * scale->y, 1.0f)
		);

		// shadow
		if (drawShadow) {
			this->material->release();
			shadowMaterial->use();
			shadowMaterial->set_texture(character.texture);

			shadowMaterial->shader->setMatrix(
			  "uTransform", glm::translate(transform, glm::vec3(0.0f, -0.2f, 0.0f))
			);

			Services::RenderingService::renderer->render_mesh(mesh);
			// Services::RenderingService::renderer->render_quad(
			// 	shadowMaterial,
			// 	glm::vec2(xPosition, yPosition) - glm::vec2(0.0f, -0.2f),
			// 	glm::vec2(glyphWidth * scale->x, glyphWidth * scale->y)
			// 	);
			shadowMaterial->release();
			this->material->use();
		}

		// regular
		this->material->shader->setMatrix("uTransform", transform);
		Services::RenderingService::renderer->render_mesh(mesh);

		renderedGlyph += 1;
	}

	// reset
	Services::RenderingService::renderer->enable_depth_buffer();
	Services::RenderingService::renderer->set_previous_depth_test();
	Services::RenderingService::renderer->set_renderer_feature(
	  Core::Rendering::RendererFeature::FaceCulling, true
	);
}

void TextLabel::update(const float deltaTime) {
	Instance::update(deltaTime);
	uiPosition.recomputeSize();
}

void TextLabel::set_text(std::string newText) {
	text = newText;
	glyphs.clear();
	calculate_text(this->text);
}

std::string TextLabel::get_text() const {
	return this->text;
}

void TextLabel::set_position(const glm::vec3 &newPosition) {
	uiPosition.setX(newPosition.x);
	uiPosition.setY(newPosition.y);

	Drawable::set_position(newPosition);

	// recalculate as they'd have to be at a new position now
	glyphs.clear();
	calculate_text(this->text);
}

Nyanners::DataTypes::Color3 TextLabel::get_color() const {
	return material->color;
}

void TextLabel::set_color(const DataTypes::Color3 &newColor) {
	material->set_color(newColor);
}

Nyanners::DataTypes::Color3 TextLabel::get_shadow_color() const {
	return this->shadowColor;
}

void TextLabel::set_shadow_color(const DataTypes::Color3 &newColor) {
	this->shadowColor = newColor;
	shadowMaterial->set_color(newColor);
}


void TextLabel::set_draw_shadow(bool shouldDraw) {
	this->drawShadow = shouldDraw;
}

bool TextLabel::get_draw_shadow() const {
	return this->drawShadow;
}

void TextLabel::calculate_text(const std::string &newText) {
	const auto window_size =
	Services::RenderingService::renderer->get_window_size();

	float globalPositionX = position->x;
	float globalPositionY = position->y;

	int renderedGlyph = 0;

	for (std::string::const_iterator iterator = newText.begin();
	     iterator != newText.end();
	     ++iterator) {
		if (renderedGlyph >= maxVisibleGlyph && maxVisibleGlyph != -1) {
			break;
		}

		const auto result = font->characters.find(*iterator);

		if (result == font->characters.end()) {
			Core::Logger::log(
			  std::format("ERROR: Unknown character {}, will not render!!", *iterator)
			);
			continue;
		}

		const auto character = result->second;

		// taken from https://youtu.be/S0PyZKX4lyI, very good watch
		if (*iterator == '\n') {
			globalPositionY -= (character.size.y * lineHeight) * textScale;
			globalPositionX = position->x;
			continue;
		}

		if (*iterator == ' ') {
			globalPositionX += (character.advance >> 6) * textScale;
			continue;
		}

		float xPosition = globalPositionX + character.bearing.x * textScale;
		float yPosition =
		  globalPositionY - (character.size.y - character.bearing.y) * textScale;

		const float glyphWidth = character.size.x * textScale;
		const float glyphHeight = character.size.y * textScale;

		DataTypes::CalculatedGlyph glyph{};
		glyph.position = glm::vec2(xPosition, yPosition);
		glyph.size = glm::vec2(glyphWidth, glyphHeight);
		glyph.character = character;

		glyphs.push_back(glyph);
		globalPositionX += (character.advance >> 6) *
		  textScale; // bitshift by 6 to get value in pixels (2^6 = 64)
	};
}
