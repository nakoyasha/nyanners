#include "RenderingService.h"
#include "IOService.h"
#include "SFML/Graphics/Font.hpp"
#include "core/Logger.h"
#include "instances/debug/DebugWindow.h"
#include "instances/drawable/Drawable.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace Nyanners::Services;

Nyanners::Resources::Shader RenderingService::defaultShader;

void RenderingService::initialize(
  const sf::VideoMode size, const std::optional<std::string> &windowTitle
) {
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antiAliasingLevel = 4;
	settings.majorVersion = 4;
	settings.minorVersion = 6;
	settings.attributeFlags = sf::ContextSettings::Core;

	if (windowTitle.has_value()) {
		window = sf::RenderWindow(
		  size,
		  windowTitle.value(),
		  sf::Style::Default,
		  sf::State::Windowed,
		  settings
		);
	} else {
		window = sf::RenderWindow(
		  size, "Nyanners", sf::Style::Default, sf::State::Windowed, settings
		);
	}

	// window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);

	if (!window.setActive(true)) {
		throw std::runtime_error("OpenGL initialiaztion failed");
	};

	const GLubyte *version = glGetString(GL_VERSION);
	const GLubyte *profile = glGetString(GL_RENDERER);

	Core::Logger::log(
	  std::format(
	    "initializing RenderingService with OpenGL {} on {}",
	    reinterpret_cast<const char *>(version),
	    reinterpret_cast<const char *>(profile)
	  )
	);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	defaultShader.load_from_file(
	  "assets/shaders/vertex.glsl", "assets/shaders/frag.glsl"
	);
	defaultShader.use();

	// Projection matrix: 45° Field of View, 4:3 ratio, display range: 0.1 unit <-> 100 units
	const auto windowSize = this->window.getSize();

	projection = glm::perspective(
	  glm::radians(45.0f),
	  static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y),
	  0.1f,
	  100.0f
	);

	// window.setMouseCursorVisible(false);
	window.setMouseCursorGrabbed(true);

	// left, right, bottom, top, zNear, zFar
	// projection = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
}

void RenderingService::start_frame() {
	const auto timeSinceLastFrame = fpsClock.restart().asSeconds();
	const auto currentFPS = 1.0f / timeSinceLastFrame;

	fps = currentFPS;

	while (const std::optional event = window.pollEvent()) {
		this->handle_window_event(event);
	}

	window.clear();
	glClear(GL_DEPTH_BUFFER_BIT);
}

void RenderingService::render(
  const std::shared_ptr<Instance> &instanceToRender
) {
	for (const auto &child : instanceToRender->children) {
		if (child->active == false) {
			continue;
		}

		const auto drawable = std::dynamic_pointer_cast<Instances::Drawable>(child);
		if (drawable == nullptr) {
			continue;
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

			glBindVertexArray(vao);
		} else {
			drawable->currentShader.use();
			drawable->currentShader.setMatrix("uModel", drawable->transform);
			drawable->currentShader.setMatrix("uView", view);
			drawable->currentShader.setMatrix("uProjection", projection);

			drawable->draw(this->window);
		}

		handle_error(instanceToRender);
	}
}

void RenderingService::end_frame() {
	window.display();
}

Nyanners::Resources::Shader RenderingService::create_default_shader() {
	Resources::Shader shader;
	shader.load_from_file(
	  "assets/shaders/vertex.glsl", "assets/shaders/frag.glsl"
	);

	return shader;
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
		const std::optional<sf::Event>& event
) {
	if (!event.has_value())
		return;

	// Close window
	if (event->is<sf::Event::Closed>()) {
		window.close();
	}

	// Resize
	else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
		projection = glm::perspective(
				glm::radians(45.0f),
				static_cast<float>(resized->size.x) /
				static_cast<float>(resized->size.y),
				0.1f,
				100.0f
		);
	}

	// Mouse look
	else if (const auto* moved = event->getIf<sf::Event::MouseMoved>()) {

		if (firstMouse) {
			lastMouse = { moved->position.x, moved->position.y };
			firstMouse = false;
		}

		float xoffset = moved->position.x - lastMouse.x;
		float yoffset = moved->position.y - lastMouse.y;

		lastMouse = { moved->position.x, moved->position.y };

		xoffset *= mouseSens;
		yoffset *= mouseSens;

		yaw   += xoffset;
		pitch -= yoffset;

		// pitch = glm::clamp(pitch, -89.0f, 89.0f);

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		cameraFront = glm::normalize(front);
	}

	Instances::DebugWindow::handle_event(window, &event.value());
}

void RenderingService::update(const float deltaTime) {
	float velocity = moveSpeed * deltaTime;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
		cameraPos += cameraFront * velocity;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		cameraPos -= cameraFront * velocity;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;

	// rebuild view
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

bool RenderingService::is_window_open() const {
	return window.isOpen();
}

GLuint RenderingService::compile_shader(
  const int shaderType, const std::filesystem::path &path
) {
	Core::Logger::log(std::format("Shader compilation: {}", path.string()));

	GLuint shaderID = glCreateShader(shaderType);
	std::string vertexShaderCode = IOService::read_file(path);

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

			Core::Logger::log(
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
	GLuint program = glCreateProgram();
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
