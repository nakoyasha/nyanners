#include "OpenGLRenderer.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "core/Logger.h"
#include "glad/glad.h"
#include "instances/drawable/MeshPart.h"
#include "utils/glCheck.h"

using namespace Nyanners::Core;

OpenGLRenderer::OpenGLRenderer(sf::Window *window) {
    currentWindow = window;
}

OpenGLRenderer::~OpenGLRenderer() {
    OpenGLRenderer::shutdown();
}

void OpenGLRenderer::initialize() {
    if (!currentWindow->setActive(true)) {
        throw std::runtime_error("OpenGL initialiaztion failed");
    };

    if (!gladLoadGLLoader(
        reinterpret_cast<GLADloadproc>(sf::Context::getFunction)
    )) {
        throw std::runtime_error("Failed to initialize GLAD");
    }

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
		set_depth_test(Core::Rendering::Less);

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    if (!ImGui_ImplOpenGL3_Init("#version 330")) {
        throw std::runtime_error("Failed to initialize ImGui");
    };

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
}

void OpenGLRenderer::start_frame() {
		clear();
}

void OpenGLRenderer::clear() {
	GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void OpenGLRenderer::render(
    const std::shared_ptr<Instances::Instance> &instanceToRender
) {
		// if (framebuffer != nullptr) {
		// 	this->framebuffer->use();
		// }

    // we can't render without a camera
    if (camera == nullptr) {
        return;
    }

    for (const auto &child: instanceToRender->renderableChildren) {
        // const auto runService = Application::instance()->currentModel->get_service<RunService>("RunService");

        child->material->shader->use();
        child->material->shader->setMatrix("uModel", child->transform);
        //
        child->material->shader->setMatrix("uView", camera->view);
        child->material->shader->setMatrix("uProjection", camera->projection);
        // child->material->shader->setFloat("iTime", runService->get_time_since_start());

        child->draw();
        handle_error(instanceToRender);
    }

	// if (framebuffer != nullptr) {
	// 	this->framebuffer->release();
	// }
}

void OpenGLRenderer::set_framerate_cap(const unsigned int framerate) {
}

void OpenGLRenderer::bind_framebuffer(Resources::FrameBuffer *newFrameBuffer) {
    if (this->framebuffer != nullptr) {
        this->unbind_framebuffer();
    }

    this->framebuffer = newFrameBuffer;
		newFrameBuffer->use();
    // const auto size = this->get_window_size();
    GL_CHECK(glViewport(0, 0, newFrameBuffer->size.x, newFrameBuffer->size.y));
}

void OpenGLRenderer::unbind_framebuffer() {
    if (this->framebuffer != nullptr) {
        this->framebuffer->release();
        this->framebuffer = nullptr;
        // delete this->framebuffer;
    }

		const auto size = this->get_window_size();
		GL_CHECK(glViewport(0, 0, size.x, size.y));
}

void OpenGLRenderer::calculate_projection(const DataTypes::Vector2 &size) {
	camera->calculate_projection(framebuffer->size);

	// TODO: make a child Camera class for ortographic? maybe put this into LayerCollector?
	projection2D = glm::ortho(0.0f, static_cast<float>(size.x), 0.0f, static_cast<float>(size.y), 1.0f, 0.0f);
}

void OpenGLRenderer::render_mesh(const Resources::Mesh *mesh) {
    mesh->bind();

    if (mesh->indexCount == 0) {
        GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, static_cast<GLint>(mesh->vertexCount)));
    } else {
        GL_CHECK(glDrawElements(
            GL_TRIANGLES, static_cast<int>(mesh->indexCount), GL_UNSIGNED_INT, nullptr
        ));
    }

    mesh->unbind();
}

void OpenGLRenderer::render_text(const std::string &text, float x, float y, const DataTypes::Color3 &color) {
}

void OpenGLRenderer::handle_event(const sf::Event *event) {
}
void OpenGLRenderer::set_depth_test(const Rendering::DepthCheckLevel& level) {
	auto currentLevel = this->lastDepthLevel;
	this->lastDepthLevel = currentLevel;

	switch (level) {
		case Rendering::DepthCheckLevel::Always:
			GL_CHECK(glDepthFunc(GL_ALWAYS));
			break;
		case Rendering::DepthCheckLevel::Never:
			GL_CHECK(glDepthFunc(GL_NEVER));
			break;
		case Rendering::DepthCheckLevel::Less:
			GL_CHECK(glDepthFunc(GL_LESS));
			break;
		case Rendering::DepthCheckLevel::Greater:
			GL_CHECK(glDepthFunc(GL_GREATER));
			break;
		case Rendering::DepthCheckLevel::Equal:
			GL_CHECK(glDepthFunc(GL_EQUAL));
			break;
		case Rendering::GreaterThanOrEqual:
			GL_CHECK(glDepthFunc(GL_GEQUAL));
			break;
		case Rendering::LessThanAndEqual:
			GL_CHECK(glDepthFunc(GL_LEQUAL));
			break;
		case Rendering::NotEqual:
			GL_CHECK(glDepthFunc(GL_NOTEQUAL));
			break;
		default:
			GL_CHECK(glDepthFunc(GL_ALWAYS));
			break;
	}
}

void OpenGLRenderer::set_previous_depth_test() {
	set_depth_test(this->lastDepthLevel);
}

void OpenGLRenderer::enable_depth_buffer() {
	glDepthMask(GL_TRUE);
}

void OpenGLRenderer::disable_depth_buffer() {
	glDepthMask(GL_FALSE);
}

void OpenGLRenderer::end_frame() {
    currentWindow->display();
}

void OpenGLRenderer::shutdown() {
    currentWindow->close();
}

Nyanners::DataTypes::Vector2 OpenGLRenderer::get_window_size() {
    if (this->framebuffer != nullptr) {
        return this->framebuffer->size;
    }

    const auto size = this->currentWindow->getSize();
    return {size.x, size.y};
}

void OpenGLRenderer::set_window_size(const DataTypes::Vector2 newWindowSize) {
    this->currentWindow->setSize({newWindowSize.x, newWindowSize.y});
}

void OpenGLRenderer::handle_error(
    const std::shared_ptr<Instances::Instance> &instanceWhereItHappened
) {
    while (const auto error = glGetError()) {
        Core::Logger::log(
            std::format(
                "Driver threw {} while rendering {}",
                glErrorToString(error),
                instanceWhereItHappened->name
            )
        );
    }
}
