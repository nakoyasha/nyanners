#include "Application.h"
#include "core/Logger.h"
#include "debug/CommandBar.h"
#include "debug/ExplorerPanel.h"
#include "debug/OutputPanel.h"
#include "debug/ViewportPanel.h"
#include "instances/Script.h"
#include "instances/debug/DebugUIService.h"
#include "instances/debug/FrameCounter.h"
#include "instances/drawable/MeshPart.h"
#include "instances/drawable/TextLabel.h"
#include "instances/services/EngineService.h"
#include "instances/services/IOService.h"
#include "instances/services/RenderingService.h"
#include "instances/services/RunService.h"
#include "instances/services/UIService.h"
#include "instances/services/user/InputService.h"
#include "instances/world/Skybox.h"
#include "instances/world/World.h"

using namespace Nyanners;
using namespace Nyanners::Services;

class TestApplication : public Application {
public:
	std::shared_ptr<Instances::Camera> camera;
	Resources::FrameBuffer *framebuffer;

	TestApplication() : Application({1280, 720}, "TestApp") {
		m_Instance = this;

		this->runService =
		  currentModel->get_service<Services::RunService>("RunService");
		this->renderService =
		  currentModel->get_service<Services::RenderingService>("RenderingService");
		this->world = currentModel->get_service<Services::World>("World");
		this->uiService =
		  currentModel->get_service<Services::UIService>("UIService");
		this->debugUI =
		  currentModel->get_service<Services::DebugUIService>("DebugUIService");
		this->camera = std::make_shared<Instances::Camera>();

		renderService->add_child(camera);
	}

	void start() override;
	void on_draw() const override;
	void on_update() override;

private:
	std::shared_ptr<RunService> runService;
	std::shared_ptr<RenderingService> renderService;
	std::shared_ptr<World> world;
	std::shared_ptr<UIService> uiService;
	std::shared_ptr<DebugUIService> debugUI;
};

void TestApplication::start() {
	RenderingService::renderer->set_current_camera(camera);
	RenderingService::renderer->set_depth_test(Core::Rendering::Always);
	RenderingService::renderer->disable_depth_buffer();

	InputService::onInput.connect([](const Input::InputEvent& event) {
		if (event.key == Input::KeyCode::F8) {

			if (event.state == Input::InputState::Began) {
				Core::Logger::log("F8 started");
			} else if (event.state == Input::InputState::Ended) {
				Core::Logger::log("F8 ended");
			}
		}
	});

	while (renderService->is_window_open()) {
		this->on_draw();
		this->on_update();
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

	renderService->bind_framebuffer(framebuffer);

	// projection has to be recalculated here, otherwise it'd look bad
	RenderingService::renderer->calculate_projection(framebuffer->size);
	RenderingService::renderer->clear();
	RenderingService::renderer->render(world);
	RenderingService::renderer->render(uiService);
	renderService->unbind_framebuffer();

	renderService->end_frame();
}

void TestApplication::on_update() {
	while (const auto event = renderService->window->pollEvent()) {
		if (!event.has_value()) {
			continue;
		}

		// TODO: better way of doing this. idk
		auto* value = &event.value();

		renderService->handle_window_event(event);
		EngineService::handle_event(value);
		InputService::handle_event(value);
	}

	runService->tick();
}

int main() {
	auto *app = new TestApplication();

	const auto model = app->currentModel;
	const auto engineService =
	  model->find_first_child<EngineService>("EngineService");

	if (engineService == nullptr) {
		EngineService::panic("Could not find EngineService??");
	}

	auto script = std::make_shared<Instances::Script>();

	try {
		auto source = IOService::read_file("assets/autorun.luau");
		script->name = "autorun";
		script->set_source(source);
		script->initialize_script();
	} catch (std::runtime_error &e) {
		EngineService::panic(
		  std::format("Failed to run autorun.luau: {}", e.what())
		);
	}

	const auto renderingService =
	  app->currentModel->get_service<Services::RenderingService>(
	    "RenderingService"
	  );
	const auto runService =
	  app->currentModel->get_service<RunService>("RunService");
	const auto uiService =
	  app->currentModel->get_service<UIService>("UIService");
	const auto world = app->currentModel->get_service<World>("World");
	const auto debugUI =
	  app->currentModel->get_service<DebugUIService>("DebugUIService");

	renderingService->initialize();
	app->framebuffer = new Resources::FrameBuffer(1280, 720);

	const auto label = std::make_shared<Instances::TextLabel>();
	const auto worldLabel = std::make_shared<Instances::TextLabel>();
	worldLabel->name = "in-world label";
	worldLabel->useWorldSpace = true;
	worldLabel->set_text("i exist in the world!");
	const auto frameCounter = std::make_shared<Debug::FrameCounter>();
	const auto mesh = std::make_shared<Instances::MeshPart>();
	const auto meshTwo = std::make_shared<Instances::MeshPart>();
	const auto teapot = std::make_shared<Instances::MeshPart>();
	// teapot->load_from_obj_file("assets/models/teapot.obj");
	// world->add_child(teapot);
	const auto skybox = std::make_shared<Instances::Skybox>();

	const auto debugWindow = std::make_shared<TestApp::Panels::ExplorerPanel>();
	const auto viewport =
	  std::make_shared<TestApp::Panels::ViewportPanel>(app->framebuffer);
	const auto commandBar = std::make_shared<TestApp::Panels::CommandBar>();
	const auto output = std::make_shared<TestApp::Panels::OutputPanel>();

	debugUI->add_child(viewport);
	debugUI->add_child(debugWindow);
	debugUI->add_child(commandBar);
	debugUI->add_child(output);

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

	mesh->material->set_texture("assets/textures/enanui.png");
	meshTwo->material->set_texture("assets/textures/saa_anyo.png");

	world->add_child(skybox);
	world->add_child(meshTwo);
	world->add_child(mesh);
	uiService->add_child(label);
	uiService->add_child(frameCounter);

	world->add_child(worldLabel);

	script->run_script();

	Core::Logger::log(std::format("Running Test App"));
	app->start();

	delete app;
	return 0;
}
