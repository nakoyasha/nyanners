#pragma once
#include "SFML/Graphics/RenderTarget.hpp"

namespace Nyanners::Instances {
  class Drawable {
  public:
    virtual bool isLegacy() {return false;};
    virtual void draw() {};
    virtual void draw(sf::RenderTarget& target) {};
  };
}
