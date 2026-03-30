#pragma once
#include "Drawable.h"
#include "Transformable.h"
#include "instances/Instance.h"
#include "instances/services/RunService.h"
#include "third_party/sfml/extlibs/headers/glad/include/glad/gl.h"
#include <filesystem>

namespace Nyanners::Instances {
  class MeshPart : public Instance, public Drawable, public Transformable {
  public:
    MeshPart();
    void draw(sf::RenderTarget& target) override;
    void update(const float deltaTime) override;
    void load_from_obj_file(const std::filesystem::path &path);
  private:
    std::vector<GLfloat> vertices {};
    GLuint vertexBuffer {};
    std::shared_ptr<Services::RunService> runService;
  };
}
