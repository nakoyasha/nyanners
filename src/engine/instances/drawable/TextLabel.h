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

    void draw(sf::RenderTarget& target) override {
      target.draw(this->label);
    }

    void update(const float deltaTime) override
    {
      // Core::Logger::log(std::format("Last frame took {}", deltaTime));
    }

    void setText(const std::string& newText)
    {
      this->text = newText;
      label.setString(newText);
    }

    std::string getText() const
    {
      return this->text;
    }

    void setFont(const sf::Font& newFont)
    {
      label.setFont(newFont);
    }

    private:
    sf::Text label;
    sf::Font font;
  };
}
