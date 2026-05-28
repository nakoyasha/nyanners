#include "Application.h"
#include "core/Logger.h"
#include "debug/CommandBar.h"
#include "debug/DebugUIService.h"
#include "debug/ExplorerPanel.h"
#include "debug/FrameCounter.h"
#include "debug/MainMenubar.h"
#include "debug/OutputPanel.h"
#include "debug/ViewportPanel.h"
#include "instances/Script.h"
#include "instances/ui/LayerCollector.h"
#include "instances/ui/Button.h"
#include "instances/ui/TextLabel.h"
#include "instances/drawable/MeshPart.h"
#include "instances/services/EngineService.h"
#include "instances/services/RenderingService.h"
#include "instances/services/RunService.h"
#include "instances/services/UIService.h"
#include "instances/services/user/InputService.h"
#include "instances/world/World.h"

using namespace Nyanners;
using namespace Nyanners::Services;

class TestApplication : public Application {
public:
	std::shared_ptr<Instances::Camera> camera;
	std::shared_ptr<Instances::Camera> cameraTwo;
	FrameBuffer *framebuffer;
	FrameBuffer *secondaryFramebuffer;
	Core::Rendering::Viewport* editorViewport;

	TestApplication() : Application({1280, 720}, "TestApp") {
		m_Instance = this;
		editorViewport = new Core::Rendering::Viewport();

		this->runService =
		  currentModel->get_service<RunService>("RunService");
		this->renderService =
		  currentModel->get_service<RenderingService>("RenderingService");
		this->world = currentModel->get_service<World>("World");
		this->uiService =
		  currentModel->get_service<UIService>("UIService");
		this->debugUI =
		  currentModel->get_service<DebugUIService>("DebugUIService");
		this->camera = std::make_shared<Instances::Camera>();
		this->cameraTwo = std::make_shared<Instances::Camera>();
		cameraTwo->useDebugMovement = false;

		camera->name = "MainCamera";
		cameraTwo->name = "SecondaryCamera";
		renderService->add_child(camera);
		renderService->add_child(cameraTwo);
	}

	void start() override;
protected:
	void on_draw() const override;
	void on_update() override;

private:
	std::shared_ptr<RunService> runService;
	std::shared_ptr<RenderingService> renderService;
	std::shared_ptr<World> world;
	std::shared_ptr<UIService> uiService;
	std::shared_ptr<DebugUIService> debugUI;
	Material *mizuMaterial;
};

void TestApplication::start() {
	RenderingService::renderer->set_current_camera(camera);
	RenderingService::renderer->set_depth_test(Core::Rendering::Always);
	RenderingService::renderer->disable_depth_buffer();
	mizuMaterial = Material::create();
	mizuMaterial->set_color({255, 255, 255, 255});
	mizuMaterial->set_texture("assets/textures/mizuzu.png");

	InputService::onInput.connect([](const Input::InputEvent &event) {
		if (event.key == Input::KeyCode::F8) {

			if (event.state == Input::InputState::Began) {
				Core::Logger::log("F8 started");
			} else if (event.state == Input::InputState::Ended) {
				Core::Logger::log("F8 ended");
			}
		}
	});

	ScriptService::run_autorun();

	while (renderService->is_window_open()) {
		this->on_update();
		this->on_draw();
	}
}

void TestApplication::on_draw() const {
	renderService->start_frame();

	// ^ start_frame might involve the user closing the window
	// therefore we stop here
	if (!renderService->is_window_open()) {
		return;
	}

	debugUI->draw_imgui();

	if (!renderService->active) {
		renderService->end_frame();
		return;
	}

	if (DebugUIService::renderWindows) {
		renderService->renderer->set_viewport(editorViewport);
		framebuffer->clear();
		RenderingService::renderer->render_from(world, nullptr, framebuffer);
		RenderingService::renderer->render_from(uiService, nullptr, framebuffer);
	} else {
		renderService->renderer->reset_viewport();
		RenderingService::renderer->render_from(world, nullptr, nullptr);
		RenderingService::renderer->render_from(uiService, nullptr, nullptr);
	}

	// re-render again into a secondary framebuffer using the 2nd camera
	secondaryFramebuffer->clear();
	RenderingService::renderer->render_from(
	  world, cameraTwo, secondaryFramebuffer
	);
	RenderingService::renderer->render_from(
	  uiService, cameraTwo, secondaryFramebuffer
	);

	renderService->end_frame();
}

void TestApplication::on_update() {
	while (const auto event = renderService->window->pollEvent()) {
		if (!event.has_value()) {
			continue;
		}

		// TODO: better way of doing this. idk
		auto *value = &event.value();

		EngineService::handle_event(value);
		renderService->handle_window_event(event);
		if (renderService->window->hasFocus()) {
			InputService::handle_event(value);
		}
	}

	runService->tick();
}

int main() {
	auto *app = new TestApplication();
	const auto model = app->currentModel;

	const auto renderingService =
	  app->currentModel->get_service<RenderingService>(
	    "RenderingService"
	  );
	const auto runService =
	  app->currentModel->get_service<RunService>("RunService");
	const auto uiService = app->currentModel->get_service<UIService>("UIService");
	const auto world = app->currentModel->get_service<World>("World");
	const auto debugUI = app->currentModel->get_service<DebugUIService>("DebugUIService");

	app->framebuffer = new FrameBuffer(1280, 720);
	app->secondaryFramebuffer = new FrameBuffer(1280, 720);

	const auto mesh = std::make_shared<Instances::MeshPart>();
	const auto meshTwo = std::make_shared<Instances::MeshPart>();
	const auto teapot = std::make_shared<Instances::MeshPart>();

	debugUI->add_child(std::make_shared<Debug::UI::ExplorerPanel>());
	debugUI->add_child(
	  std::make_shared<Debug::UI::ViewportPanel>(app->framebuffer, app->editorViewport)
	);
	debugUI->add_child(std::make_shared<Debug::UI::CommandBar>());
	debugUI->add_child(std::make_shared<Debug::UI::OutputPanel>());
	debugUI->add_child(std::make_shared<Debug::UI::MainMenubar>());

	mesh->set_vertices(
	  {-0.5f,
	   -0.5f,
	   0.0f,
	   0.0f,
	   0.5f,
	   -0.5f,
	   1.0f,
	   0.0f,
	   0.5f,
	   0.5f,
	   1.0f,
	   1.0f,
	   -0.5f,
	   0.5f,
	   0.0f,
	   1.0f}
	);

	mesh->name = "ena";
	meshTwo->name = "saa anyo";

	// 0 and 2 are duplicates; therefore it can be optimized down here:
	mesh->set_indexes({0, 1, 2, 2, 3, 0});
	mesh->set_color({255, 255, 255, 255});

	meshTwo->set_vertices(
	  {-0.5f,
	   -0.5f,
	   0.0f,
	   0.0f,
	   0.5f,
	   -0.5f,
	   1.0f,
	   0.0f,
	   0.5f,
	   0.5f,
	   1.0f,
	   1.0f,
	   -0.5f,
	   0.5f,
	   0.0f,
	   1.0f}
	);

	// 0 and 2 are duplicates; therefore it can be optimized down here:
	meshTwo->set_indexes({0, 1, 2, 2, 3, 0});
	meshTwo->set_color({255, 255, 255, 255});

	mesh->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
	meshTwo->set_position(glm::vec3(0.0f, 2.0f, 0.0f));

	world->add_child(meshTwo, mesh);
	mesh->material->set_texture("assets/textures/enanui.png");
	meshTwo->material->set_texture(app->secondaryFramebuffer->framebufferTexture);

	Core::Logger::log("Running Test App");
	app->start();

	delete app;
	return 0;
}
