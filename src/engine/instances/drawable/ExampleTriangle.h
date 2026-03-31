#pragma once
#include "Drawable.h"
#include "instances/Instance.h"
#include "third_party/sfml/extlibs/headers/glad/include/glad/gl.h"

namespace Nyanners::Instances {
  class ExampleTriangle : public Instance, public Drawable {
  public:
    ExampleTriangle();

    void draw(sf::RenderTarget &target) override;
  private:
    const std::vector<GLfloat> g_vertex_buffer_data;
    GLuint vertexBuffer{};
  };
}
