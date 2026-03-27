#include "Application.h"

#include "core/Logger.h"

void Nyanners::Application::start() const {
  Core::Logger::log(std::format("Nyanners running"));
}

void Nyanners::Application::shutdown() {
  this->currentModel = nullptr;
}

Nyanners::Application::~Application() {
  this->shutdown();
}