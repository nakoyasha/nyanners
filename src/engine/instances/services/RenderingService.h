#pragma once
#include "SFML/Graphics/RenderWindow.hpp"
#include "instances/Instance.h"
#include "resources/FrameBuffer.h"
#include "resources/Shader.h"
#include "third_party/sfml/extlibs/headers/glad/include/glad/gl.h"
#include <glm/gtc/matrix_transform.hpp>
#include <filesystem>

namespace Nyanners::Services
{
    class RenderingService : public Instances::Instance
    {
        public:
        static Resources::Shader defaultShader;
        sf::RenderWindow window;
    		std::unique_ptr<Resources::FrameBuffer> framebuffer;

        float deltaTime = 0.0f;
        int fps = 0.0f;

        // Camera matrix
        glm::mat4 view = glm::lookAt(
            glm::vec3(4,5,3), // Camera is at (4,3,3), in World Space
            glm::vec3(0,0,0), // and looks at the origin
            glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
            );


        RenderingService() : Instance("RenderingService") {};

        void initialize(
          sf::VideoMode size, const std::optional<std::string> &windowTitle
        );
        void set_window_title(const std::string& newWindowTitle);
        void set_fps_limit(const unsigned int limit);
        void set_resolution(const sf::Vector2u newSize);
    		void bind_framebuffer(const Resources::FrameBuffer& framebuffer);
        void handle_window_event(const std::optional<sf::Event> &event);
		    void update(const float deltaTime) override;
		    bool is_window_open() const;

        // shaders
        static GLuint compile_shader(const int shaderType = GL_VERTEX_SHADER, const std::filesystem::path &path = "assets/shaders/vertex.glsl");
        static GLuint compile_program(const GLuint vertex, const GLuint fragment);

        void start_frame();
        void render(const std::shared_ptr<Instance> &instanceToRender);
    		void render_to_framebuffer(const std::shared_ptr<Instance> &instanceToRender);
    		static Resources::Shader create_default_shader();
        void end_frame();
        void shutdown();
    private:
    		static void handle_error(const std::shared_ptr<Instance> whereItHappened);

        glm::mat4 projection;
        sf::Clock fpsClock;

    		// Camera state
    		glm::vec3 cameraPos   = { 0.0f, 0.0f, 3.0f };
    		glm::vec3 cameraFront = { 0.0f, 0.0f, -1.0f };
    		glm::vec3 cameraUp    = { 0.0f, 1.0f, 0.0f };

    		float yaw   = -90.0f; // look forward
    		float pitch = 0.0f;

    		float moveSpeed = 6.0f;
    		float mouseSens = 0.1f;

    		bool firstMouse = true;
    		sf::Vector2i lastMouse;
    };
}
