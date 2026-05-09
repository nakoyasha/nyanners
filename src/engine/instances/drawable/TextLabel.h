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
    int maxVisibleGlyph = -1;
  	bool useWorldSpace = false;

		TextLabel();
  	~TextLabel();
    void draw() override;
    void update(const float deltaTime) override;

    void set_text(const std::string& newText);
    void set_position(const glm::vec3 &newPosition) override;
  private:
    // TODO: Abstract this away into a "Font" resource
  	Resources::Font* font;
  	std::vector<DataTypes::CalculatedGlyph> glyphs;

    float scale = 1;
    float lineHeight = 1.3;


    DataTypes::Vertices vertices;
  	void calculate_text(const std::string& text);
  };
}
