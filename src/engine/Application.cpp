#include "Application.h"

#include "core/ServiceProvider.h"
#ifdef INCLUDE_DEBUG_UI_SERVICE
#include "debug/DebugUIService.h"
#endif
#include "instances/services/AssetService.h"
#include "instances/services/EngineService.h"
#include "instances/services/RenderingService.h"
#include "instances/services/RunService.h"
#include "instances/services/SoundService.h"
#include "instances/services/UIService.h"
#include "instances/services/light/LightingService.h"
#include "instances/services/user/InputService.h"
#include "instances/world/World.h"
#include "serialization/ProjectParser.h"

using namespace Nyanners;

Application::Application() : project("none", "none", "none") {
	const auto provider = Services::ServiceProvider::instance();

	provider->add_service<Services::ReflectionService>();
	provider->add_service<Services::SoundService>();
	provider->add_service(Services::InputService::instance());
	provider->add_service<Services::EngineService>();
	provider->add_service<Services::RunService>();

	Services::ReflectionService::register_reflections();
	Application::set_datamodel(Serialization::ProjectParser::make_blank_data_model());
}

Application::~Application() {
	this->Application::shutdown();
}

void Application::start() {
	const auto run = this->currentModel->get_service<Services::RunService>("RunService");
	run->run();
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

void Application::shutdown() {
	running = false;
	Services::ServiceProvider::instance()->get_service<Services::RunService>("RunService")->stop();

	if (has_rendering) {
		Services::RenderingService::instance()->shutdown();
	}

	this->currentModel.reset();
	this->currentModel = nullptr;
}

void Application::load_default_project() {
	this->load_project(defaultProject);
}

void Application::load_from_project_file(const std::filesystem::path &path) {
	const auto newProject = Serialization::ProjectParser::load_project_from_file(path);
	Services::AssetService::instance()->set_asset_root(path.parent_path());
	load_project(newProject);
}

void Application::load_project(const Core::Project &newProject) {
	Core::Logger::log_debug(std::format("Loading project {}", newProject.name));
	this->project = newProject;

	if (is_rendering_enabled()) {
		Services::RenderingService::instance()->set_window_title(this->project.name);
	}

	if (this->project.initScene != "none") {
		set_datamodel(this->project.load_init_scene());
	}
}

void Application::set_datamodel(const std::shared_ptr<DataModel>& model) {
	assert(model != nullptr);
	const auto provider = Services::ServiceProvider::instance();
	provider->get_service<Services::RunService>("RunService")->bind_model(model);

	for (const auto &service: provider->services | std::views::values) {
		model->add_child(service);
	}

	this->currentModel = std::move(model);
	this->onDataModelSwitch.fire(this->currentModel);
}

void Application::init_rendering(const DataTypes::Vector2 &size, const std::string &windowTitle) {
	const auto service = Services::RenderingService::instance();
	const auto provider = Services::ServiceProvider::instance();
	const auto lighting = Services::LightingService::instance();

	service->initialize(size, windowTitle);
	has_rendering = true;
	provider->add_service(service);
	provider->add_service(lighting);

#ifdef INCLUDE_DEBUG_UI_SERVICE
	const auto debug = provider->add_service<Services::DebugUIService>();
	debug->add_standard_elements();
#endif
}

bool Application::is_rendering_enabled() const {
	return this->has_rendering;
}

void Application::on_update() {
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

void Application::on_draw() const {
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

	const auto size = renderService->renderer->get_window_size();
	renderService->renderer->bind_framebuffer(renderService->renderer->defaultFramebuffer);
	renderService->renderer->defaultFramebuffer->resize(size.x, size.y);
	renderService->renderer->clear();



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