#include "Application.h"
#include "core/Logger.h"
#include "debug/DebugUIService.h"
#include "instances/services/EngineService.h"
#include "instances/services/RenderingService.h"
#include "instances/services/RunService.h"
#include "instances/services/ScriptService.h"
#include "instances/services/SelectionService.h"
#include "instances/services/SoundService.h"
#include "instances/services/UIService.h"
#include "instances/services/user/InputService.h"
#include "instances/world/World.h"

Nyanners::Application::Application(const DataTypes::Vector2 size, const std::string &windowTitle) {
	renderService = std::make_shared<Services::RenderingService>(size, windowTitle);
	reflectionService = std::make_shared<Services::ReflectionService>();
	soundService = std::make_shared<Services::SoundService>();
	inputService = std::make_shared<Services::InputService>();
	runService = std::make_shared<Services::RunService>();
	engineService = std::make_shared<Services::EngineService>();

	Services::ReflectionService::register_reflections();
	Application::set_datamodel(make_datamodel());
}

void Nyanners::Application::start() {
	while (renderService->is_window_open()) {
		this->on_update();
		this->on_draw();
	}
}

void Nyanners::Application::shutdown() {
	runService->stop();
	renderService->shutdown();

	this->currentModel.reset();
	this->currentModel = nullptr;
}

void Nyanners::Application::set_datamodel(const std::shared_ptr<Instances::DataModel>& model) {
	assert(model != nullptr);
	runService->bind_model(model);
	model->add_child(runService);

	model->add_child(renderService);
	model->add_child(soundService);
	model->add_child(reflectionService);
	model->add_child(inputService);
	model->add_child(runService);
	model->add_child(engineService);

	this->currentModel = std::move(model);
	this->onDataModelSwitch.fire(this->currentModel);
}

std::shared_ptr<Nyanners::Instances::DataModel> Nyanners::Application::make_datamodel() {
	const auto model = std::make_shared<Instances::DataModel>();

	model->add_child(std::make_shared<Services::SelectionService>());
	model->add_child(std::make_shared<Services::UIService>());
	model->add_child(std::make_shared<Services::World>());
	model->add_child(std::make_shared<Services::ScriptService>());
	model->add_child(std::make_shared<Services::IOService>());
	model->add_child(std::make_shared<Services::DebugUIService>());

	return model;
}

Nyanners::Application::~Application() {
	this->Application::shutdown();
}
