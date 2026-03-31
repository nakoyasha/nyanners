#include "RenderingService.h"
#include "IOService.h"
#include "SFML/Graphics/Font.hpp"
#include "core/Logger.h"
#include "instances/debug/DebugWindow.h"
#include "instances/drawable/Drawable.h"
#include "instances/drawable/Transformable.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace Nyanners::Services;

void RenderingService::initialize(
  const sf::VideoMode size, const std::optional<std::string> &windowTitle
) {
  sf::ContextSettings settings;
  settings.depthBits = 24;
  settings.stencilBits = 8;
  settings.antiAliasingLevel = 4;
  settings.majorVersion = 4;
  settings.minorVersion = 6;
  // settings.attributeFlags = sf::ContextSettings::Core;

  if (windowTitle.has_value()) {
    window = sf::RenderWindow(size, windowTitle.value(), sf::Style::Default, sf::State::Windowed, settings);
  } else {
    window = sf::RenderWindow(size, "Nyanners", sf::Style::Default, sf::State::Windowed, settings);
  }

  window.setVerticalSyncEnabled(true);
  window.setFramerateLimit(60);

  if (!window.setActive(true)) {
    throw std::runtime_error("OpenGL initialiaztion failed");
  };

  const GLubyte* version = glGetString(GL_VERSION);
  const GLubyte* profile = glGetString(GL_RENDERER);

  Core::Logger::log(std::format("initializing RenderingService with OpenGL {} on {}", reinterpret_cast<const char*>(version), reinterpret_cast<const char*>(profile)));

  defaultShader.load_from_file("assets/shaders/vertex.glsl", "assets/shaders/frag.glsl");
  defaultShader.use();
  currentShader = defaultShader;

  // Projection matrix: 45° Field of View, 4:3 ratio, display range: 0.1 unit <-> 100 units
  const auto windowSize = this->window.getSize();

  projection = glm::perspective(glm::radians(45.0f), static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y), 0.1f, 100.0f);
}

void RenderingService::render(
  const std::shared_ptr<Instance>& instanceToRender
) {
  for (const auto& child : instanceToRender->children) {

    if (child->active == false) {
      continue;
    }

    const auto drawable = std::dynamic_pointer_cast<Instances::Drawable>(child);
    if (drawable == nullptr) {
      continue;
    }


    if (const auto transformable = std::dynamic_pointer_cast<Instances::Transformable>(drawable)) {
      const glm::mat4 mvp = this->projection * this->view * transformable->transform;
      defaultShader.setMatrix("transform", mvp);
    }

    if (drawable->isLegacy()) {
      // absolutely jank code, god this is awful.
      GLint vao;
      glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vao);
      glBindVertexArray(0);
      glUseProgram(0);

      this->window.pushGLStates();
      drawable->draw(this->window);
      this->window.popGLStates();

      currentShader.use();
      glBindVertexArray(vao);
    } else {
      drawable->draw(this->window);
    }
  }
}
void RenderingService::end_frame() {
  if (const auto error = glGetError(); error != GL_NO_ERROR) {
    if (error == GL_INVALID_ENUM) {
      Core::Logger::log("Driver threw GL_INVALID_ENUM while rendering");
    } else if (error == GL_INVALID_OPERATION) {
      Core::Logger::log("Driver threw GL_INVALID_OPERATION while rendering");
    } else if (error == GL_INVALID_VALUE) {
      Core::Logger::log("Driver threw GL_INVALID_VALUE while rendering");
    } else if (error == GL_INVALID_FRAMEBUFFER_OPERATION) {
      Core::Logger::log("Driver threw GL_INVALID_FRAMEBUFFER_OPERATION while rendering");
    } else if (error == GL_OUT_OF_MEMORY) {
      Core::Logger::log("Driver threw GL_OUT_OF_MEMORY while rendering");
    } else if (error == GL_STACK_UNDERFLOW) {
      Core::Logger::log("Driver threw GL_STACK_UNDERFLOW while rendering");
    } else if (error == GL_STACK_OVERFLOW) {
      Core::Logger::log("Driver threw GL_STACK_OVERFLOW while rendering");
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
  const std::optional<sf::Event> &event
) {
  // Close window: exit
  if (event->is<sf::Event::Closed>()) {
    window.close();
  } else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
  }

  if (event.has_value()) {
    Instances::DebugWindow::handle_event(this->window, &event.value());
  }
}

bool RenderingService::is_window_open() const {
  return window.isOpen();
}

GLuint RenderingService::compile_shader(
  const int shaderType, const std::filesystem::path &path
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

    Core::Logger::log(std::format("Error while compiling {}: {}", path.string(), &VertexShaderErrorMessage[0]));
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
void RenderingService::start_frame() {
  const auto timeSinceLastFrame = fpsClock.restart().asSeconds();
  const auto currentFPS = 1.0f / timeSinceLastFrame;

  fps = currentFPS;

  while (const std::optional event = window.pollEvent()) {
    this->handle_window_event(event);
  }

  window.clear();
}

void RenderingService::shutdown() {
  this->window.close();
}
