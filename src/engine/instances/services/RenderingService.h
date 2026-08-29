#pragma once
#include "core/rendering/Renderer.h"
#include "instances/Instance.h"
#include "instances/datatypes/Vector.h"
#include "resources/FrameBuffer.h"
#include "resources/Shader.h"
#include <filesystem>

namespace Nyanners::Rendering {
    enum class RenderingBackend {
        OpenGL = 0,
        // TODO: vulkan? idk
    };
}

namespace Nyanners::Services {
    class RenderingService : public Instances::Instance, public Service<RenderingService> {
    public:
        float deltaTime = 0.0f;
        float frameTime = 0.0f;
        int fps = 0;

        static Rendering::RenderingBackend backend;
        static std::vector<std::shared_ptr<Resources::Texture> > textures;
        static std::unique_ptr<Core::Renderer> renderer;

        sf::Window *window;
        // TODO: make this a list of shaders instead, with priorities on when they should execute
        std::shared_ptr<Resources::Shader> postProcessShader;

        RenderingService() : Instance("RenderingService") {};

        void initialize(DataTypes::Vector2, const std::optional<std::string> &windowTitle, Rendering::RenderingBackend withBackend = Rendering::RenderingBackend::OpenGL);
        void set_window_title(const std::string &newWindowTitle) const;
        void set_fps_limit(const unsigned int limit);
        void set_resolution(const sf::Vector2u newSize);
        void bind_framebuffer(Resources::FrameBuffer *framebuffer);
        void unbind_framebuffer();
        void handle_window_event(const std::optional<sf::Event> &event);
        bool is_window_open() const;

        [[nodiscard]] double get_fps() const { return static_cast<double>(this->fps); }
        [[nodiscard]] glm::vec2 get_window_size() const;

        static void add_texture(const std::shared_ptr<Resources::Texture> &texture);
        static void remove_texture(const std::shared_ptr<Resources::Texture> &texture);

        // shaders
        static GLuint compile_shader(const int shaderType = GL_VERTEX_SHADER,const std::filesystem::path &path = "assets/shaders/vertex.glsl");
        static GLuint compile_program(const GLuint vertex, const GLuint fragment);
        static Resources::Shader create_default_shader();
        std::shared_ptr<Resources::Shader> create_shader(const std::filesystem::path& vertex, const std::filesystem::path& fragment);
        void set_post_process_shader(const std::shared_ptr<Resources::Shader>& shader);
        void clear_post_process_shader();

        void start_frame();
        void end_frame();
        void shutdown();
    private:
        sf::Clock fpsClock;
        bool inFocus = true;

        static void handle_error(const std::shared_ptr<Instance> whereItHappened);
    };
}
