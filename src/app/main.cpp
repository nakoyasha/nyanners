#include "Application.h"
#include "core/Logger.h"
#include "debug/ExplorerPanel.h"
#include "debug/ViewportPanel.h"
#include "instances/Script.h"
#include "instances/drawable/MeshPart.h"
#include "instances/drawable/TextLabel.h"
#include "instances/services/EngineService.h"
#include "instances/services/IOService.h"
#include "instances/services/RenderingService.h"
#include "instances/services/RunService.h"
#include "instances/services/UIService.h"
#include "instances/debug/DebugUIService.h"
#include "instances/world/Skybox.h"
#include "instances/world/World.h"

using namespace Nyanners;
using namespace Nyanners::Services;

class TestApplication : public Application {
	public:
	TestApplication() : Application() {
		this->runService = currentModel->get_service<Services::RunService>("RunService");
		this->renderService = currentModel->get_service<Services::RenderingService>("RenderingService");
		this->world = currentModel->get_service<Services::World>("World");
		this->uiService = currentModel->get_service<Services::UIService>("UIService");
		this->debugUI = currentModel->get_service<Services::DebugUIService>("DebugUIService");
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

}

void TestApplication::on_draw() const {
	renderService->start_frame();

	// ^ start_frame might involve the user closing the window
	// therefore we stop here
	if (!renderService->is_window_open()) {
		return;
	}

	renderService->render(uiService);
	renderService->render(world);
	debugUI->draw_imgui(renderService->window);

	renderService->end_frame();
}

void TestApplication::on_update() {
	runService->tick();
}

int main() {
  auto* app = TestApplication::instance();

  const auto model = app->currentModel;
  const auto engineService = model->find_first_child<EngineService>("EngineService");

  if (engineService == nullptr) {
    EngineService::panic("Could not find EngineService??");
  }

  auto script = std::make_shared<Instances::Script>();

  try {
    auto source = IOService::read_file("assets/autorun.luau");
    Core::Logger::log(source);
    script->name = "autorun";
    script->set_source(source);
    script->initialize_script();
  } catch (std::runtime_error& e) {
    EngineService::panic(std::format("Failed to run autorun.luau: {}", e.what()));
  }

  const auto renderingService = app->currentModel->get_service<Services::RenderingService>("RenderingService");
  const auto runService = app->currentModel->get_service<Services::RunService>("RunService");
  const auto uiService = app->currentModel->get_service<Services::UIService>("UIService");
  const auto world = app->currentModel->get_service<Services::World>("World");
	const auto debugUI = app->currentModel->get_service<Services::DebugUIService>("DebugUIService");

  renderingService->initialize(sf::VideoMode({1280, 720}), "Test App");

  auto label = std::make_shared<Instances::TextLabel>();
  auto mesh = std::make_shared<Instances::MeshPart>();
	auto meshTwo = std::make_shared<Instances::MeshPart>();
	auto skybox = std::make_shared<Instances::Skybox>();

  // mesh->transform = glm::translate(mesh->transform, glm::vec3(0.0f, 5.0f, 0.0f));
  auto debugWindow = std::make_shared<TestApp::Panels::ExplorerPanel>();
	// auto viewport = std::make_shared<TestApp::Panels::ViewportPanel>();

	// debugUI->add_child(viewport);
  debugUI->add_child(debugWindow);

	mesh->set_vertices({
		-0.5f, -0.5f, 0.0f, 0.0f,
		0.5f, -0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.0f, 1.0f
	});

	mesh->name = "ena";
	meshTwo->name = "saa anyo";

	// 0 and 2 are duplicates; therefore it can be optimized down here:
	mesh->set_indexes({0, 1, 2, 2, 3, 0});
	// mesh->set_color({187, 221, 34});

	meshTwo->set_vertices({
		-0.5f, -0.5f, 0.0f, 0.0f,
		0.5f, -0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.0f, 1.0f
	});

	// 0 and 2 are duplicates; therefore it can be optimized down here:
	meshTwo->set_indexes({0, 1, 2, 2, 3, 0});
	meshTwo->set_color({187, 221, 34});

	mesh->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
	meshTwo->set_position(glm::vec3(0.0f, 2.0f, -1.0f));

	mesh->texture->load_from_file("assets/textures/enanui.png");
	meshTwo->texture->load_from_file("assets/textures/saa_anyo.png");
	// meshTwo->texture->set_mipmap_enabled(false);

	// debugWindow->onImmediateRender->connect([renderingService, meshTwo]() {
		// static float newPosition[3] = {meshTwo->position.x, meshTwo->position.y, meshTwo->position.z};
		// static glm::vec3 cameraView = glm::vec3(renderingService->view[3]);
		// static glm::vec3 camRot = glm::vec3(0.0f); // pitch, yaw, roll
		//
		// if (ImGui::SliderFloat3("Position", newPosition, -24, 24)) {
		// 	meshTwo->set_position({
		// 			newPosition[0],
		// 			newPosition[1],
		// 			newPosition[2]
		// 	});
		// };
		//
		// if (ImGui::SliderFloat3("Camera View", &cameraView.x, -1024, 1024)) {
		// 	renderingService->view[3][0] = cameraView[0];
		// }

	// });

	world->add_child(skybox);
	world->add_child(meshTwo);
  world->add_child(mesh);

  const sf::Font font("C:/Windows/Fonts/arial.ttf");
  sf::Text text(font, "Hello SFML", 50);

  script->run_script();

  Core::Logger::log(std::format("Running Test App"));
  app->start();

  // while (runService->isRunning == true && renderingService->is_window_open())
  // {
  //   renderingService->render(label);
  // }

  delete app;
  return 0;
}