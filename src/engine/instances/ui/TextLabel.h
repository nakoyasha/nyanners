#pragma once

#include "UIElement.h"
#include "instances/Instance.h"
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
  class TextLabel : public UIElement {
  public:
    int maxVisibleGlyph = -1;
  	int textSize = 48;
  	bool useWorldSpace = false;
  	bool drawShadow = false;

    DataTypes::UDim2 uiPosition;
  	DataTypes::Color3 shadowColor {10, 10, 10, 80};
    std::string text = "hello world";

  	TextLabel();
  	~TextLabel() override;
    void draw() override;

    void set_text(std::string newText);
  	[[nodiscard]] std::string get_text() const;
    void set_position(const glm::vec3 &newPosition) override;

  	DataTypes::Color3 get_color() const;
  	void set_color(const DataTypes::Color3 &newColor);
  	DataTypes::Color3 get_shadow_color() const;
  	void set_shadow_color(const DataTypes::Color3 &newColor);
  	void set_draw_shadow(bool shouldDraw);
  	bool get_draw_shadow() const;

    double get_text_size() const;
  	void set_text_size(double newTextSize);

  	void update(float deltaTime) override;
  private:
    // TODO: Abstract this away into a "Font" resource
    float textScale = 1;
    float lineHeight = 1.3;

  	Resources::Font* font;
  	Resources::Material* shadowMaterial;
  	std::vector<DataTypes::CalculatedGlyph> glyphs;

  	void calculate_text(const std::string& text);
  };
}
