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

		TextLabel();
    void draw() override;
    void update(const float deltaTime) override;

    void set_text(const std::string& newText);
    void set_position(const glm::vec3 &newPosition) override;
  private:
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
    FT_Library ft;
    std::map<char, DataTypes::Character> characters;
    float scale = 1;

    // TODO: Abstract this away into a "Font" resource
    FT_Face fontFace;
    int fontHeight = 48;
    int fontWidth = 0;

    std::vector<Resources::Mesh> meshes;
    void generate_character(unsigned char vChar);
  };
}
