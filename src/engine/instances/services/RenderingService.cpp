#include "RenderingService.h"
#include "IOService.h"
#include "SFML/Graphics/Font.hpp"
#include "core/Logger.h"
#include "instances/drawable/Drawable.h"
#include "instances/drawable/Transformable.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace Nyanners::Services;

void RenderingService::initialize(
  const sf::VideoMode size, const std::optional<std::string> windowTitle
) {
  if (windowTitle.has_value()) {
    window = sf::RenderWindow(size, windowTitle.value());
  } else {
    window = sf::RenderWindow(size, "Nyanners");
  }

  window.setVerticalSyncEnabled(true);
  window.setFramerateLimit(60);

  const GLubyte* version = glGetString(GL_VERSION);
  const GLubyte* profile = glGetString(GL_RENDERER);

  Core::Logger::log(std::format("initializing RenderingService with OpenGL {} on {}", reinterpret_cast<const char*>(version), reinterpret_cast<const char*>(profile)));

  glGenVertexArrays(1, &vertexArrayID);
  glBindVertexArray(vertexArrayID);

  defaultShader.load_from_file("assets/shaders/vertex.glsl", "assets/shaders/frag.glsl");
  defaultShader.use();
  currentShader = defaultShader;

  // Projection matrix: 45° Field of View, 4:3 ratio, display range: 0.1 unit <-> 100 units
  const auto windowSize = this->window.getSize();

  projection = glm::perspective(glm::radians(45.0f), (float) windowSize.x / (float)windowSize.y, 0.1f, 100.0f);
}

void RenderingService::render(
  std::shared_ptr<Instances::Instance> instanceToRender
) {
  const auto timeSinceLastFrame = fpsClock.restart().asSeconds();
  const auto currentFPS = 1.0f / timeSinceLastFrame;

  fps = currentFPS;

  while (const std::optional event = window.pollEvent()) {
    this->handle_window_event(event);
  }

  window.clear();

  for (auto child : instanceToRender->children) {
    const auto drawable = std::dynamic_pointer_cast<Instances::Drawable>(child);
    if (drawable == nullptr) {
      continue;
    }

    if (auto transformable = std::dynamic_pointer_cast<Instances::Transformable>(drawable)) {
      const glm::mat4 mvp = this->projection * this->view * transformable->transform;
      defaultShader.setMatrix("transform", mvp);
    }

    if (drawable->isLegacy()) {
      // absolutely jank code, god this is awful.
      glBindVertexArray(0);
      glUseProgram(0);

      this->window.pushGLStates();
      drawable->draw(this->window);
      this->window.popGLStates();

      currentShader.use();
      glBindVertexArray(this->vertexArrayID);
    } else {
      drawable->draw(this->window);
    }

  }


  window.display();
}

void RenderingService::set_window_title(const std::string &newWindowTitle) {
  window.setTitle(newWindowTitle);
}

void RenderingService::set_fps_limit(const unsigned int limit) {
  this->window.setFramerateLimit(limit);
}

void RenderingService::set_resolution(const sf::Vector2u newSize) {
  this->window.setSize(newSize);
}

void RenderingService::handle_window_event(
  const std::optional<sf::Event> event
) {
  // Close window: exit
  if (event->is<sf::Event::Closed>()) {
    window.close();
  }
}

bool RenderingService::is_window_open() const {
  return window.isOpen();
}

GLuint RenderingService::compile_shader(
  const int shaderType, const std::filesystem::path path
) {
  // Create the shaders
  GLuint shaderID = glCreateShader(shaderType);

  // Read the Vertex Shader code from the file
  std::string vertexShaderCode = IOService::read_file(path);

  GLint Result = GL_FALSE;
  int infoLogLength;

  // Compile Vertex Shader
  Core::Logger::log(std::format("Shader compilation: {}", path.string()));

  char const *sourceRaw = vertexShaderCode.c_str();
  glShaderSource(shaderID, 1, &sourceRaw, nullptr);
  glCompileShader(shaderID);

  // Check Vertex Shader
  glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
  if (infoLogLength > 0) {
    std::vector<char> VertexShaderErrorMessage(infoLogLength + 1);
    glGetShaderInfoLog(
      shaderID, infoLogLength, nullptr, &VertexShaderErrorMessage[0]
    );

    Core::Logger::log(&VertexShaderErrorMessage[0]);
  }

  return shaderID;
}
GLuint RenderingService::compile_program(const GLuint vertex, const GLuint fragment) {
  GLuint program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  glLinkProgram(program);

  GLint result = GL_FALSE;
  int infoLength;
  glGetProgramiv(program, GL_LINK_STATUS, &result);
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLength);

  if ( infoLength > 0 ){
    std::vector<char> ProgramErrorMessage(infoLength+1);
    glGetProgramInfoLog(program, infoLength, nullptr, &ProgramErrorMessage[0]);

    Core::Logger::log(&ProgramErrorMessage[0]);
  }

  glDetachShader(program, vertex);
  glDetachShader(program, fragment);

  glDeleteShader(vertex);
  glDeleteShader(fragment);

  return program;
}

void RenderingService::shutdown() {
  this->window.close();
}
