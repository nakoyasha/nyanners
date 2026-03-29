#include "Application.h"

#include "core/Logger.h"
#include "instances/drawable/TextLabel.h"
#include "instances/services/EngineService.h"
#include "instances/services/RenderingService.h"
#include "instances/services/RunService.h"
#include "instances/services/UIService.h"

Nyanners::Application::Application()
{
  auto model = std::make_shared<Instances::DataModel>();
  auto runService = std::make_shared<Services::RunService>();
  runService->bind_model(model);
  model->add_child(runService);
  model->add_child(std::make_shared<Services::EngineService>());
  model->add_child(std::make_shared<Services::IOService>());
  model->add_child(std::make_shared<Services::RenderingService>());
  model->add_child(std::make_shared<Services::ReflectionService>());
  model->add_child(std::make_shared<Services::UIService>());

  Services::ReflectionService::register_reflections();

  this->currentModel = std::move(model);
}

void Nyanners::Application::start() const {
  Core::Logger::log(std::format("Nyanners running"));
  const auto runService = this->currentModel->get_service<Services::RunService>("RunService");
  runService->run();
}

void Nyanners::Application::shutdown() {
  const auto renderingService = this->currentModel->get_service<Services::RenderingService>("RenderingService");
  const auto runService = this->currentModel->get_service<Services::RunService>("RunService");
  runService->stop();
  renderingService->shutdown();

  this->currentModel = nullptr;
}

Nyanners::Application::~Application() {
  this->Application::shutdown();
}