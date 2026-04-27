#pragma once

#include "MeshPart.h"
#include "core/Logger.h"
#include "freetype/freetype.h"
#include "instances/datatypes/UDim2.h"

namespace Nyanners::DataTypes {
  struct Character {
    std::shared_ptr<Resources::Texture> texture;
    glm::ivec2 size;
    glm::ivec2 bearing;
    long advance; // next glyph offset
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
    void draw() override;
    void update(const float deltaTime) override;

    void set_text(const std::string& newText);
    void set_position(const glm::vec3 &newPosition) override;
  private:
    FT_Library ft;
    std::map<char, DataTypes::Character> characters;
    float scale = 1;
    float lineHeight = 1.3;

    // TODO: Abstract this away into a "Font" resource
    FT_Face fontFace;
    int fontHeight = 48;
    int fontWidth = 0;

    DataTypes::Vertices vertices;
    void generate_character(unsigned char vChar);
  };
}
