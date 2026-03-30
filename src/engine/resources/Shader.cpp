#include "Shader.h"
#include "instances/services/RenderingService.h"

using namespace Nyanners::Resources;

void Shader::load_from_file(
  const std::filesystem::path &vertexPath, const std::filesystem::path &fragmentPath
) {
  const auto vertexShader = Services::RenderingService::compile_shader(GL_VERTEX_SHADER, vertexPath);
  const auto fragmentShader = Services::RenderingService::compile_shader(GL_FRAGMENT_SHADER, fragmentPath);

  const auto programId = Services::RenderingService::compile_program(vertexShader, fragmentShader);
  shaderId = programId;
}

void Shader::setBool(const std::string &name, const bool value) const {
  glUniform1i(glGetUniformLocation(shaderId, name.c_str()), (int)value);
}
void Shader::setInt(const std::string &name, const int value) const {
  glUniform1i(glGetUniformLocation(shaderId, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, const float value) const {
  glUniform1f(glGetUniformLocation(shaderId, name.c_str()), value);
}

void Shader::setMatrix(const std::string &name, const glm::mat4 value) const {
  glUniformMatrix4fv(glGetUniformLocation(shaderId, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

Shader::~Shader() {
  glDeleteProgram(shaderId);
  shaderId = 0xDEADBEEF;
}

void Shader::use() const {
  if (shaderId == 0xDEADBEEF) {
    throw std::runtime_error("Cannot use shader while unloaded");
  }

  glUseProgram(this->shaderId);
}