#pragma once
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Window.hpp"
#include "instances/Camera.h"
#include "instances/Instance.h"
#include "instances/datatypes/Vector.h"
#include "resources/FrameBuffer.h"
#include "scripting/reflections/DataTypes.h"
#include <utility>

namespace Nyanners::Core::Rendering {
	enum DepthCheckLevel {
		Always = 0,
		Never = 1,
		Equal = 2,
		Less = 3,
		Greater = 4,
		LessThanAndEqual = 5,
		GreaterThanOrEqual = 6,
		NotEqual = 7,
	};

	enum class RendererFeature {
		FaceCulling = 0,
		DepthTesting = 1,
		Blending = 2,
	};
}

namespace Nyanners::Core {

	class Renderer {
	public:
		virtual ~Renderer() = default;
		Resources::FrameBuffer* framebuffer = nullptr;
		std::shared_ptr<Instances::Camera> camera;
		glm::mat4 projection2D = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);

		virtual void initialize() = 0;
		virtual void start_frame() = 0;
		virtual void clear() = 0;
		virtual void render(const std::shared_ptr<Instances::Instance> &instanceToRender) = 0;
		virtual void render_from(const std::shared_ptr<Instances::Instance> &root, std::shared_ptr<Instances::Camera> camera, Resources::FrameBuffer* framebuffer) = 0;
		virtual void set_current_camera(std::shared_ptr<Instances::Camera> newCamera) {
			camera = std::move(newCamera);
		}
		virtual void bind_framebuffer(Resources::FrameBuffer* newFrameBuffer) = 0;
		virtual void calculate_projection(const DataTypes::Vector2& size, std::shared_ptr<Instances::Camera> camera) = 0;
		virtual void unbind_framebuffer() = 0;
		virtual void render_mesh(const Resources::Mesh* mesh) = 0;

		// 2d
		virtual void render_quad(Resources::Material* material, const glm::vec2& position, const glm::vec2& size) = 0;
		// 3d
		virtual void render_quad(Resources::Material* material, const glm::vec3& position, const glm::vec2& size) = 0;
		virtual void set_depth_test(const Rendering::DepthCheckLevel&) = 0;
		virtual void set_previous_depth_test() = 0;
		virtual void set_renderer_feature(Rendering::RendererFeature feature, bool enabled) = 0;
		virtual void enable_depth_buffer() = 0;
		virtual void disable_depth_buffer() = 0;

		virtual void handle_event(const sf::Event* event) = 0;
		virtual void end_frame() = 0;
		virtual void shutdown() = 0;

		virtual DataTypes::Vector2 get_window_size() = 0;
		virtual void set_window_size(const DataTypes::Vector2 newWindowSize) = 0;

		static std::unique_ptr<Renderer> create(sf::Window* window);
	};
}