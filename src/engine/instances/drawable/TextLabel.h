#pragma once
#include "Drawable.h"
#include "core/Logger.h"
#include "instances/Instance.h"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Text.hpp"

namespace Nyanners::Instances {
  class TextLabel : public Instance, public Drawable {
  public:
    std::string text;

    TextLabel() : Instance("TextLabel"), font("C:/Windows/Fonts/arial.ttf"), label(font)
    {
      this->label.setCharacterSize(50);
      this->label.setString("hi!");
    };

    bool isLegacy() override {
      return true;
    }

    void draw() override;
    void setText(const std::string& newText);

    std::string getText() const;

    void setFont(const sf::Font& newFont);

  private:
    sf::Text label;
    sf::Font font;
  };
}
