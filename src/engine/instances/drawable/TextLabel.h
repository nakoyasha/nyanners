#pragma once

#include "MeshPart.h"
#include "instances/datatypes/UDim2.h"
#include "resources/Font.h"

namespace Nyanners::DataTypes {
	struct CalculatedGlyph {
		Character character;
		glm::vec2 position;
		glm::vec2 size;
	};
}

namespace Nyanners::Instances {
  class TextLabel : virtual public Instance, public Drawable {
  public:
    std::string text = "hello world";
    DataTypes::UDim2 uiPosition;
  	DataTypes::Color3 shadowColor {10, 10, 10, 80};
    int maxVisibleGlyph = -1;
  	bool useWorldSpace = false;
  	bool drawShadow = false;

		TextLabel();
  	~TextLabel();
    void draw() override;
    void update(const float deltaTime) override;

    void set_text(const std::string& newText);
    void set_position(const glm::vec3 &newPosition) override;

  	void set_shadow_color(const DataTypes::Color3 &newColor);
		DataTypes::Color3 get_shadow_color() const;
  	void set_draw_shadow(bool shouldDraw);
  	bool get_draw_shadow() const;
  private:
    // TODO: Abstract this away into a "Font" resource
  	Resources::Font* font;
  	std::vector<DataTypes::CalculatedGlyph> glyphs;
  	Resources::Material* shadowMaterial;

    float textScale = 1;
    float lineHeight = 1.3;

  	void calculate_text(const std::string& text);
  };
}
