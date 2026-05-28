#include "Application.h"
#include "core/Logger.h"
#include "debug/DebugUIService.h"
#include "instances/services/EngineService.h"
#include "instances/services/RenderingService.h"
#include "instances/services/RunService.h"
#include "instances/services/ScriptService.h"
#include "instances/services/SelectionService.h"
#include "instances/services/UIService.h"
#include "instances/services/user/InputService.h"
#include "instances/world/World.h"

Nyanners::Application::Application(const DataTypes::Vector2 size, const std::string& windowTitle)
{
  auto model = std::make_shared<Instances::DataModel>();
  const auto runService = std::make_shared<Services::RunService>();
  runService->bind_model(model);
  model->add_child(runService);

  model->add_child(std::make_shared<Services::EngineService>());
	model->add_child(std::make_shared<Services::SelectionService>());
  model->add_child(std::make_shared<Services::IOService>());
  model->add_child(std::make_shared<Services::RenderingService>(size, windowTitle));
  model->add_child(std::make_shared<Services::ReflectionService>());
	model->add_child(std::make_shared<Services::DebugUIService>());
  model->add_child(std::make_shared<Services::UIService>());
  model->add_child(std::make_shared<Services::World>());
	model->add_child(std::make_shared<Services::InputService>());
	model->add_child(std::make_shared<Services::ScriptService>());

  Services::ReflectionService::register_reflections();

  this->currentModel = std::move(model);
}

void Nyanners::Application::start() {
  Core::Logger::log(std::format("Nyanners running"));
  const auto runService = this->currentModel->get_service<Services::RunService>("RunService");
  runService->run();
}

void Nyanners::Application::shutdown() {
  const auto renderingService = this->currentModel->get_service<Services::RenderingService>("RenderingService");
  const auto runService = this->currentModel->get_service<Services::RunService>("RunService");
  runService->stop();
  renderingService->shutdown();

	this->currentModel.reset();
  this->currentModel = nullptr;
}

Nyanners::Application::~Application() {
  this->Application::shutdown();
}