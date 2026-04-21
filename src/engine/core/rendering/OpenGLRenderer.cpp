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
    glDepthMask(GL_TRUE);

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::render(
    const std::shared_ptr<Instances::Instance> &instanceToRender
) {
    if (framebuffer != nullptr) {
        glDisable(GL_DEPTH_TEST);
        framebuffer->use();
    }

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

    if (framebuffer != nullptr) {
        framebuffer->release();
        glEnable(GL_DEPTH_TEST);
    }
}

void OpenGLRenderer::set_framerate_cap(const unsigned int framerate) {
}

void OpenGLRenderer::bind_framebuffer(Resources::FrameBuffer *newFrameBuffer) {
    if (this->framebuffer != nullptr) {
        this->unbind_framebuffer();
    }

    this->framebuffer = newFrameBuffer;
    const auto size = this->get_window_size();
    glViewport(0, 0, size.x, size.y);
}

void OpenGLRenderer::unbind_framebuffer() {
    if (this->framebuffer != nullptr) {
        this->framebuffer->release();
        this->framebuffer = nullptr;
        delete this->framebuffer;
    }
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
