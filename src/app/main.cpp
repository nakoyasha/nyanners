#include "Application.h"
#include "core/Logger.h"
#include "instances/Script.h"
#include "instances/debug/DebugWindow.h"
#include "instances/drawable/MeshPart.h"
#include "instances/drawable/TextLabel.h"
#include "instances/services/EngineService.h"
#include "instances/services/IOService.h"
#include "instances/services/RenderingService.h"
#include "instances/services/RunService.h"
#include "instances/services/UIService.h"
#include "instances/world/World.h"

using namespace Nyanners;
using namespace Nyanners::Services;

int main() {
  auto* app = Application::instance();

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

  renderingService->initialize(sf::VideoMode({1280, 720}), "Test App");

  auto label = std::make_shared<Instances::TextLabel>();
  auto mesh = std::make_shared<Instances::MeshPart>();
  auto debugWindow = std::make_shared<Instances::DebugWindow>();

  uiService->add_child(label);
  uiService->add_child(debugWindow);
  // uiService->add_child(triangle);
  mesh->load_from_obj_file("assets/models/teapot.obj");
  world->add_child(mesh);

  const sf::Font font("C:/Windows/Fonts/arial.ttf");
  sf::Text text(font, "Hello SFML", 50);

  script->run_script();

  app->start();
  Core::Logger::log(std::format("Running Test App"));

  // while (runService->isRunning == true && renderingService->is_window_open())
  // {
  //   renderingService->render(label);
  // }

  delete app;
  return 0;
}