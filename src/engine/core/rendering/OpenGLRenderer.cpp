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
	// TODO: move initialization to constructor
}

OpenGLRenderer::~OpenGLRenderer() {
	OpenGLRenderer::shutdown();
}

void OpenGLRenderer::initialize() {
	if (!currentWindow->setActive(true)) {
		throw std::runtime_error("OpenGL initialization failed");
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
	    "initializing OpenGL {} on {}",
	    reinterpret_cast<const char *>(version),
	    reinterpret_cast<const char *>(profile)
	  )
	);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	if (!ImGui_ImplOpenGL3_Init("#version 330")) {
		throw std::runtime_error("Failed to initialize ImGui");
	};

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	set_depth_test(Core::Rendering::Less);
	quadMesh = Resources::Mesh::create();
	quadMesh->bind();
	quadMesh->set_vertices({
		-0.5f, -0.5f, 0.0f, 0.0f,
		0.5f,-0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, 1.0f, 1.0f,
		-0.5f, 0.5f,0.0f, 1.0f
	});

	quadMesh->set_indexes({0, 1, 2, 2, 3, 0});
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
	quadMesh->unbind();
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
	// we can't render without a camera
	if (camera == nullptr) {
		return;
	}

	for (const auto &child : instanceToRender->renderableChildren) {
		if (auto drawable = child.lock()) {
			if (std::dynamic_pointer_cast<Instances::Instance>(drawable)->active == false) {
				continue;
			}

			drawable->material->shader->use();
			drawable->material->shader->setMatrix("uModel", drawable->get_transform());
			drawable->material->shader->setMatrix("uView", camera->view);
			drawable->material->shader->setMatrix("uProjection", camera->projection);

			drawable->draw();
		}
	}
}
void OpenGLRenderer::render_from(
  const std::shared_ptr<Instances::Instance> &root,
  const std::shared_ptr<Instances::Camera> camera,
  Resources::FrameBuffer *framebuffer
) {
	if (root->active == false) {
		return;
	}

	std::weak_ptr<Instances::Camera> activeCamera;

	if (camera != nullptr) {
		activeCamera = camera;
	} else {
		activeCamera = this->camera;
	}


	if (auto cCam = activeCamera.lock(); framebuffer != nullptr) {
		bind_framebuffer(framebuffer);

		// resize as this will be a rendertarget
		if (cCam == this->camera) {
			cCam->resolution = framebuffer->size;
		} else {
			framebuffer->resize(cCam->resolution->x, cCam->resolution->y);
		}
	} else {
		cCam->resolution = new DataTypes::Vector2(get_window_size());
	}

	if (const auto usedCamera = activeCamera.lock()) {
		calculate_projection(get_window_size(), activeCamera.lock());

		for (const auto &child : root->renderableChildren) {
			// TODO: optimize this somehow. idk a better way to do this

			if (auto drawable = child.lock()) {
				if (std::dynamic_pointer_cast<Instances::Instance>(drawable)->active == false) {
					continue;
				}

				drawable->material->shader->use();
				drawable->material->shader->setMatrix("uModel", drawable->get_transform());
				drawable->material->shader->setMatrix("uView", usedCamera->view);
				drawable->material->shader->setMatrix("uProjection", usedCamera->projection);

				drawable->draw();
			}

			handle_error(root);
		}
	}


	unbind_framebuffer();
}

void OpenGLRenderer::bind_framebuffer(Resources::FrameBuffer *newFrameBuffer) {
	if (this->framebuffer != nullptr) {
		this->unbind_framebuffer();
	}

	this->framebuffer = newFrameBuffer;
	newFrameBuffer->use();
	GL_CHECK(glViewport(0, 0, newFrameBuffer->size->x, newFrameBuffer->size->y));
}

void OpenGLRenderer::unbind_framebuffer() {
	if (this->framebuffer != nullptr) {
		this->framebuffer->release();
		this->framebuffer = nullptr;
	}

	const auto size = this->get_window_size();
	GL_CHECK(glViewport(0, 0, size.x, size.y));
}

void OpenGLRenderer::calculate_projection(const DataTypes::Vector2 &size, const std::shared_ptr<Instances::Camera> camera) {
	camera->calculate_projection(size, false);

	// TODO: make a child Camera class for ortographic? maybe put this into LayerCollector?
	projection2D = glm::ortho(
	  0.0f,
	  static_cast<float>(size.x),
	  0.0f,
	  static_cast<float>(size.y),
	  1.0f,
	  0.0f
	);
}

void OpenGLRenderer::set_renderer_feature(Rendering::RendererFeature feature, bool enabled) {
	if (enabled) {
		switch (feature) {
			case Rendering::RendererFeature::FaceCulling:
				glEnable(GL_CULL_FACE);
				break;
			case Rendering::RendererFeature::DepthTesting:
				glEnable(GL_DEPTH_TEST);;
				break;
			case Rendering::RendererFeature::Blending:
				glEnable(GL_BLEND);
				break;
			default:
				throw std::runtime_error("set_renderer_feature on unknown feature");
		}
	} else {
		switch (feature) {
			case Rendering::RendererFeature::FaceCulling:
				glDisable(GL_CULL_FACE);
				break;
			case Rendering::RendererFeature::DepthTesting:
				glDisable(GL_DEPTH_TEST);;
				break;
			case Rendering::RendererFeature::Blending:
				glDisable(GL_BLEND);
				break;
			default:
				throw std::runtime_error("set_renderer_feature on unknown feature");
		}
	}

}

void OpenGLRenderer::render_mesh(const Resources::Mesh *mesh) {
	mesh->bind();

	if (mesh->indexCount == 0) {
		GL_CHECK(
		  glDrawArrays(GL_TRIANGLES, 0, static_cast<GLint>(mesh->vertexCount))
		);
	} else {
		GL_CHECK(glDrawElements(
		  GL_TRIANGLES, static_cast<int>(mesh->indexCount), GL_UNSIGNED_INT, nullptr
		));
	}

	mesh->unbind();
}
void OpenGLRenderer::render_quad(
  Resources::Material *material,
  const glm::vec2 &position,
  const glm::vec2 &size
) {
	const auto transform = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0.0f));

	material->use();
	material->shader->setMatrix("uModel", glm::scale(transform, glm::vec3(size.x, size.y, 0.0f)));
	material->shader->setMatrix("uView", camera->view);
	material->shader->setMatrix("uProjection", camera->projection);
	material->shader->setBool("uScreenSpace", true);

	quadMesh->bind();
	render_mesh(quadMesh);
	quadMesh->unbind();
	material->release();
}

void OpenGLRenderer::render_quad(
  Resources::Material *material,
  const glm::vec3 &position,
  const glm::vec2 &size
) {
	const auto transform = glm::translate(glm::mat4(1.0f), position);

	material->use();
	material->shader->setMatrix("uModel", glm::scale(transform, glm::vec3(size.x, size.y, 0.0f)));
	material->shader->setMatrix("uView", camera->view);
	material->shader->setMatrix("uProjection", camera->projection);
	material->shader->setBool("uScreenSpace", false);

	quadMesh->bind();
	render_mesh(quadMesh);
	quadMesh->unbind();
	material->release();
}

void OpenGLRenderer::handle_event(const sf::Event *event) {
	if (const auto *resized = event->getIf<sf::Event::Resized>()) {
		calculate_projection({resized->size.x, resized->size.y}, camera);
	}
}

void OpenGLRenderer::set_depth_test(const Rendering::DepthCheckLevel &level) {
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
		return *this->framebuffer->size;
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
