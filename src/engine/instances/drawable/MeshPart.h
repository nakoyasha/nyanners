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
    void draw(sf::RenderTarget &target) override;
    void update(const float deltaTime) override;
    void load_from_obj_file(const std::filesystem::path &path);
  private:
    std::vector<GLfloat> vertices {};
    std::vector<GLfloat> texCoords {
      // https://learnopengl.com/Getting-started/Textures
      0.0f, 0.0f,  // lower-left corner
      1.0f, 0.0f,  // lower-right corner
      0.5f, 1.0f   // top-center corner
    };
    GLuint vertexBuffer {};
    GLuint vertexArrayID;
    unsigned int textureId;
    std::shared_ptr<Services::RunService> runService;
  };
}
