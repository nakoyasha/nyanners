#pragma once
#include "instances/datatypes/Color3.h"
#include "third_party/sfml/extlibs/headers/glad/include/glad/gl.h"
#include <glm/gtc/matrix_transform.hpp>
#include <filesystem>

namespace Nyanners::Resources {
  class Shader {
  public:
    Shader() = default;
    ~Shader();

    void use() const;
  	void release() const;
    void load_from_file(
      const std::filesystem::path &vertexPath,
      const std::filesystem::path &fragmentPath
    );

    void setBool(const std::string &name, const bool value) const;
    void setInt(const std::string &name, const int value) const;
    void setFloat(const std::string &name, const float value) const;
    void setMatrix(const std::string &name, const glm::mat4 &value) const;
    void setColor(const std::string &name, const Nyanners::DataTypes::Color3 value) const;

  private:
    GLuint shaderId = 0xDEADBEEF;
  };
}
