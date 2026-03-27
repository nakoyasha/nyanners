#include "Application.h"

#include "core/Logger.h"
#include "instances/drawable/TextLabel.h"

void Nyanners::Application::start() const {
  Core::Logger::log(std::format("Nyanners running"));
}

void Nyanners::Application::shutdown() {
  const auto renderingService = this->currentModel->get_service<Services::RenderingService>("RenderingService");
  renderingService->shutdown();
  this->currentModel = nullptr;
}

Nyanners::Application::~Application() {
  this->shutdown();
}