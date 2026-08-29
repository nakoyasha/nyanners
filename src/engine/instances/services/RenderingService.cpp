#define STB_IMAGE_IMPLEMENTATION
#include "RenderingService.h"
#include "Application.h"
#include "io/IOService.h"
#include "RunService.h"
#include "stb_image.h"
#include "debug/DebugUIService.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/VideoMode.hpp"
#include "core/Logger.h"

using namespace Nyanners::Services;

std::unique_ptr<Nyanners::Core::Renderer> RenderingService::renderer;
std::vector<std::shared_ptr<Nyanners::Resources::Texture>> RenderingService::textures;
Nyanners::Rendering::RenderingBackend RenderingService::backend;

void RenderingService::initialize(DataTypes::Vector2 size, const std::optional<std::string> &windowTitle,
	Rendering::RenderingBackend withBackend) {
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antiAliasingLevel = 4;
	settings.majorVersion = 4;
	settings.minorVersion = 6;
	settings.attributeFlags = sf::ContextSettings::Core;

	const auto videoMode = sf::VideoMode({size.x, size.y});

	if (windowTitle.has_value()) {
		window = new sf::Window(
		  videoMode,
		  windowTitle.value(),
		  sf::Style::Default,
		  sf::State::Windowed,
		  settings
		);
	} else {
		window = new sf::Window(
		  videoMode, "Nyanners", sf::Style::Default, sf::State::Windowed, settings
		);
	}

	window->setFramerateLimit(120);
	backend = withBackend;

	renderer = Core::Renderer::create(window);
	renderer->initialize();
}

void RenderingService::start_frame() {
	const auto timeSinceLastFrame = fpsClock.restart().asSeconds();
	const int currentFPS = std::floor(1.0f / timeSinceLastFrame);

	fps = currentFPS;
	frameTime = timeSinceLastFrame;
	renderer->start_frame();
}

void RenderingService::end_frame() {
	DebugUIService::on_frame_end();
	renderer->end_frame();
}

Nyanners::Resources::Shader RenderingService::create_default_shader() {
	Resources::Shader shader;
	shader.load_from_file(
	  "assets/shaders/vertex.glsl", "assets/shaders/frag.glsl"
	);

	return shader;
}

std::shared_ptr<Nyanners::Resources::Shader> RenderingService::create_shader(const std::filesystem::path &vertex, const std::filesystem::path &fragment) {
	for (const auto& child : peek_at<Resources::Shader>("Shader")) {
		const auto shader = child.lock();

		if (shader->vertexPath == vertex && shader->fragmentPath == fragment) {
			return shader;
		}
	}

	auto shader = std::make_shared<Resources::Shader>();
	shader->load_from_file(vertex, fragment);
	add_child(shader);

	return shader;
}

void RenderingService::set_window_title(const std::string &newWindowTitle
) const {
	// glfwSetWindowTitle(window, newWindowTitle.c_str());
	window->setTitle(newWindowTitle);
}

void RenderingService::set_fps_limit(const unsigned int limit) {
	this->window->setFramerateLimit(limit);
}

void RenderingService::set_resolution(const sf::Vector2u newSize) {
	this->window->setSize(newSize);
}
void RenderingService::bind_framebuffer(
  Resources::FrameBuffer *framebuffer
) {
	// this->framebuffer = framebuffer;
	renderer->bind_framebuffer(framebuffer);
}

void RenderingService::unbind_framebuffer() {
	renderer->unbind_framebuffer();
}

void RenderingService::handle_window_event(
		const std::optional<sf::Event>& event
) {
	if (!event.has_value())
		return;

	if (!window->hasFocus())
		return;

	// Close window
	if (event->is<sf::Event::Closed>()) {
		window->close();
	}

	if (event->is<sf::Event::FocusGained>()) {
		inFocus = true;
	} else if (event->is<sf::Event::FocusLost>()) {
		inFocus = false;
	}

	if (!inFocus) {
		return;
	}

	DebugUIService::handle_event(window, &event.value());
}

bool RenderingService::is_window_open() const {
	return window->isOpen();
}

glm::vec2 RenderingService::get_window_size() const {
	return this->renderer->camera->resolution;
}

void RenderingService::add_texture(
  const std::shared_ptr<Resources::Texture> &texture
) {
	textures.push_back(texture);
}

void RenderingService::remove_texture(
  const std::shared_ptr<Resources::Texture> &texture
) {
	std::erase(textures, texture);
}

GLuint RenderingService::compile_shader(
  const int shaderType, const std::filesystem::path &path
) {
	const GLuint shaderID = glCreateShader(shaderType);
	const std::string vertexShaderCode = IOService::instance()->read_file(path);
	Core::Logger::log(std::format("Shader compilation: {}", path.string()));

	const char *sourceRaw = vertexShaderCode.c_str();
	glShaderSource(shaderID, 1, &sourceRaw, nullptr);
	glCompileShader(shaderID);

	int compileResult;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileResult);

	if (compileResult == GL_FALSE) {
		int infoLogLength;

		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0) {
			std::vector<char> shaderErrorMessage(infoLogLength + 1);

			glGetShaderInfoLog(
			  shaderID, infoLogLength, nullptr, &shaderErrorMessage[0]
			);

			Core::Logger::log_error(
			  std::format(
			    "Error while compiling {}: {}", path.string(), &shaderErrorMessage[0]
			  )
			);
		}
	}

	return shaderID;
}

GLuint
RenderingService::compile_program(const GLuint vertex, const GLuint fragment) {
	const GLuint program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	glValidateProgram(program);

	GLint result = GL_FALSE;
	int infoLength;
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLength);

	if (infoLength > 0) {
		std::vector<char> ProgramErrorMessage(infoLength + 1);
		glGetProgramInfoLog(program, infoLength, nullptr, &ProgramErrorMessage[0]);

		Core::Logger::log(&ProgramErrorMessage[0]);

		return -1;
	}

	glDetachShader(program, vertex);
	glDetachShader(program, fragment);

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return program;
}

void RenderingService::shutdown() {
	this->window->close();
}

constexpr const char *glErrorToString(GLenum error) {
	switch (error) {
		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM";
		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION";
		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE";
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return "GL_INVALID_FRAMEBUFFER_OPERATION";
		case GL_OUT_OF_MEMORY:
			return "GL_OUT_OF_MEMORY";
		case GL_STACK_UNDERFLOW:
			return "GL_STACK_UNDERFLOW";
		case GL_STACK_OVERFLOW:
			return "GL_STACK_OVERFLOW";
		default:
			return "UNKNOWN_GL_ERROR";
	}
}

void RenderingService::handle_error(
  const std::shared_ptr<Instance> whereItHappened
) {
	while (const auto error = glGetError()) {
		Core::Logger::log(
		  std::format(
		    "Driver threw {} while rendering {}",
		    glErrorToString(error),
		    whereItHappened->name
		  )
		);
	}
}
