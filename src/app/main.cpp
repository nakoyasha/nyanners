#include "Application.h"
#include "core/Logger.h"
#include "instances/Script.h"
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
    script->run_script();
  } catch (std::runtime_error& e) {
    EngineService::panic(std::format("Failed to run autorun.luau: {}", e.what()));
  }

  delete app;
  return 0;
}