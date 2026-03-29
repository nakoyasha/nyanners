#pragma once
#include "SFML/Graphics/RenderTarget.hpp"

namespace Nyanners::Instances {
  class Drawable {
  public:
    virtual void draw(sf::RenderTarget& target) {};
  };
}
