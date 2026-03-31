#pragma once
#include "SFML/Graphics/RenderWindow.hpp"
#include "instances/Instance.h"
#include "resources/Shader.h"
#include "third_party/sfml/extlibs/headers/glad/include/glad/gl.h"
#include <glm/gtc/matrix_transform.hpp>
#include <filesystem>

namespace Nyanners::Services
{
    class RenderingService : public Instances::Instance
    {
        public:
        float deltaTime = 0.0f;
        int fps = 0.0f;

        RenderingService() : Instance("RenderingService") {};

        void initialize(
          sf::VideoMode size, const std::optional<std::string> &windowTitle
        );
        void set_window_title(const std::string& newWindowTitle);
        void set_fps_limit(const unsigned int limit);
        void set_resolution(const sf::Vector2u newSize);
        void handle_window_event(const std::optional<sf::Event> &event);
        bool is_window_open() const;

        // shaders
        static GLuint compile_shader(const int shaderType = GL_VERTEX_SHADER, const std::filesystem::path &path = "assets/shaders/vertex.glsl");
        static GLuint compile_program(const GLuint vertex, const GLuint fragment);

        void start_frame();
        void render(const std::shared_ptr<Instance> &instanceToRender);
        void end_frame();
        void shutdown();
    private:
        sf::RenderWindow window;
        Resources::Shader currentShader;
        glm::mat4 projection;
        glm::vec3 cameraPos;
        sf::Clock fpsClock;
        Resources::Shader defaultShader;

        // Camera matrix
        glm::mat4 view = glm::lookAt(
            glm::vec3(4,5,3), // Camera is at (4,3,3), in World Space
            glm::vec3(0,0,0), // and looks at the origin
            glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
            );
    };
}
