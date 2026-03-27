#include "Application.h"
#include "core/Logger.h"
#include "instances/Script.h"
#include "instances/drawable/TextLabel.h"
#include "instances/services/EngineService.h"
#include "instances/services/IOService.h"

using namespace Nyanners;
using namespace Nyanners::Services;

int main() {
  auto* app = Application::instance();
  app->start();
  Core::Logger::log(std::format("Running Test App"));

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
    script->source = source;
  } catch (std::runtime_error& e) {
    EngineService::panic(std::format("Failed to run autorun.luau: {}", e.what()));
  }

  const auto renderingService = app->currentModel->get_service<Services::RenderingService>("RenderingService");
  auto label = std::make_shared<Instances::TextLabel>();
  app->currentModel->add_child(label);

  const sf::Font font("C:/Windows/Fonts/arial.ttf");
  sf::Text text(font, "Hello SFML", 50);

  renderingService->initialize(sf::VideoMode({1280, 720}), "Test App");

  script->run_script();

  while (renderingService->is_window_open())
  {
    renderingService->render(label);
  }

  delete app;
  return 0;
}