#include "Application.h"

#include "core/ServiceProvider.h"
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

Nyanners::Application::Application() {
	// reflectionService = std::make_shared<Services::ReflectionService>();
	// soundService = std::make_shared<Services::SoundService>();
	// runService = std::make_shared<Services::RunService>();
	// engineService = std::make_shared<Services::EngineService>();

	const auto provider = Services::ServiceProvider::instance();

	provider->add_service<Services::ReflectionService>();
	provider->add_service<Services::SoundService>();
	provider->add_service(Services::InputService::instance());
	provider->add_service<Services::EngineService>();
	provider->add_service<Services::RunService>();

	Services::ReflectionService::register_reflections();
	Application::set_datamodel(make_datamodel());
}

void Nyanners::Application::start() {
	running = true;

	while (running) {
		this->on_update();
		if (has_rendering && Services::RenderingService::instance()->is_window_open()) {
			this->on_draw();
		} else {
			break;
		}

	}
}

void Nyanners::Application::shutdown() {
	running = false;
	Services::ServiceProvider::instance()->get_service<Services::RunService>("RunService")->stop();
	Services::RenderingService::instance()->shutdown();

	this->currentModel.reset();
	this->currentModel = nullptr;
}

void Nyanners::Application::set_datamodel(const std::shared_ptr<Instances::DataModel>& model) {
	assert(model != nullptr);
	const auto provider = Services::ServiceProvider::instance();
	provider->get_service<Services::RunService>("RunService")->bind_model(model);

	for (const auto &service: provider->services | std::views::values) {
		model->add_child(service);
	}

	this->currentModel = std::move(model);
	this->onDataModelSwitch.fire(this->currentModel);
}

void Nyanners::Application::init_rendering(const DataTypes::Vector2 &size, const std::string &windowTitle) {
	const auto service = Services::RenderingService::instance();
	const auto provider = Services::ServiceProvider::instance();

	service->initialize(size, windowTitle);
	has_rendering = true;
	provider->add_service(service);

	// this should probably be handled better, but whatever...
	this->currentModel->add_child(service);
}

bool Nyanners::Application::is_rendering_enabled() const {
	return this->has_rendering;
}

std::shared_ptr<Nyanners::Instances::DataModel> Nyanners::Application::make_datamodel() {
	const auto model = std::make_shared<Instances::DataModel>();

	model->add_child(std::make_shared<Services::SelectionService>());
	model->add_child(std::make_shared<Services::UIService>());
	model->add_child(std::make_shared<Services::World>());
	model->add_child(std::make_shared<Services::ScriptService>());
	model->add_child(std::make_shared<Services::IOService>());

	// model->add_child(std::make_shared<Services::DebugUIService>());


	return model;
}

Nyanners::Application::~Application() {
	this->Application::shutdown();
}
