#include "Application.h"

#include "core/ServiceProvider.h"
#ifdef INCLUDE_DEBUG_UI_SERVICE
#include "debug/DebugUIService.h"
#endif
#include "instances/services/EngineService.h"
#include "instances/services/RenderingService.h"
#include "instances/services/RunService.h"
#include "instances/services/ScriptService.h"
#include "instances/services/SelectionService.h"
#include "instances/services/SoundService.h"
#include "instances/services/UIService.h"
#include "instances/services/io/IOService.h"
#include "instances/services/user/InputService.h"
#include "instances/world/World.h"

Nyanners::Application::Application() {
	const auto provider = Services::ServiceProvider::instance();

	provider->add_service<Services::ReflectionService>();
	provider->add_service<Services::SoundService>();
	provider->add_service(Services::InputService::instance());
	provider->add_service<Services::EngineService>();
	provider->add_service<Services::RunService>();

	Services::ReflectionService::register_reflections();
	Application::set_datamodel(make_datamodel());
}

Nyanners::Application::~Application() {
	this->Application::shutdown();
}

void Nyanners::Application::start() {
	auto run = this->currentModel->get_service<Services::RunService>("RunService");
	running = true;

	while (running) {
		this->on_update();
		if (has_rendering) {
			const auto render = Services::RenderingService::instance();
			if (!render->is_window_open()) {
				return;
			}

			run->preRender->fire(render->deltaTime);
			render->start_frame();

			// ^ start_frame might involve the user closing the window
			// therefore we stop here
			if (!render->is_window_open()) {
				return;
			}

			this->on_draw();
			run->onRender->fire(render->deltaTime);
			render->end_frame();
		} else {
			break;
		}

	}
}

void Nyanners::Application::shutdown() {
	running = false;
	Services::ServiceProvider::instance()->get_service<Services::RunService>("RunService")->stop();

	if (has_rendering) {
		Services::RenderingService::instance()->shutdown();
	}

	this->currentModel.reset();
	this->currentModel = nullptr;
}

void Nyanners::Application::set_datamodel(const std::shared_ptr<DataModel>& model) {
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

#ifdef INCLUDE_DEBUG_UI_SERVICE
	const auto debug = provider->add_service<Services::DebugUIService>();
	debug->add_standard_elements();
#endif
}

bool Nyanners::Application::is_rendering_enabled() const {
	return this->has_rendering;
}

std::shared_ptr<DataModel> Nyanners::Application::make_datamodel() {
	const auto model = std::make_shared<DataModel>();

	model->add_child(std::make_shared<Services::SelectionService>());
	model->add_child(std::make_shared<Services::UIService>());
	model->add_child(std::make_shared<Services::World>());
	model->add_child(std::make_shared<Services::ScriptService>());
	model->add_child(std::make_shared<Services::IOService>());

	return model;
}

void Nyanners::Application::on_update() {
	const auto runService = Services::ServiceProvider::instance()->get_service<Services::RunService>("RunService");

	if (has_rendering) {
		const auto renderService = Services::RenderingService::instance();

		// TODO: Decouple event polling from RenderingService, somehow? maybe? if possible at all even
		while (const auto event = renderService->window->pollEvent()) {
			if (!event.has_value()) {
				continue;
			}

			// TODO: better way of doing this. idk
			auto *value = &event.value();

			Services::EngineService::handle_event(value);
			renderService->handle_window_event(event);
			if (renderService->window->hasFocus()) {
				Services::InputService::instance()->handle_event(value);
			}
		}
	}

	runService->tick();
}

void Nyanners::Application::on_draw() const {
	const auto world = currentModel->get_service<Services::World>("World");
	const auto uiService = currentModel->get_service<Services::UIService>("UIService");
	const auto renderService = Services::RenderingService::instance();

#ifdef INCLUDE_DEBUG_UI_SERVICE
	const auto debugUI = currentModel->get_service<Services::DebugUIService>("DebugUIService");
	debugUI->draw_imgui();
#endif

	if (!renderService->active) {
		return;
	}

#ifdef INCLUDE_DEBUG_UI_SERVICE
	auto framebuffer = renderService->renderer->defaultFramebuffer;
	auto viewport = renderService->renderer->currentViewport;

	if (debugUI->renderWindows) {
		framebuffer = debugUI->viewportFramebuffer;
		viewport = debugUI->debugViewport;
	}
#endif

	// 	auto size = renderService->renderer->get_window_size();
	// renderService->renderer->bind_framebuffer(renderService->renderer->defaultFramebuffer);
	// renderService->renderer->clear();

	renderService->renderer->set_viewport(viewport);
	renderService->renderer->bind_framebuffer(framebuffer);
	renderService->renderer->clear();

	renderService->renderer->render_from(world, nullptr, framebuffer);
	renderService->renderer->render_from(uiService, nullptr, framebuffer);
	renderService->renderer->bind_framebuffer(framebuffer);
	renderService->renderer->unbind_framebuffer();
	renderService->renderer->render_post_process(framebuffer, renderService->postProcessingShaders);

	renderService->renderer->reset_viewport();
}