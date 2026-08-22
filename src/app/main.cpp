#include "Application.h"
#include "core/Logger.h"
#include "debug/CommandBar.h"
#include "debug/DebugUIService.h"
#include "debug/ExplorerPanel.h"
#include "debug/MainMenubar.h"
#include "debug/OutputPanel.h"
#include "debug/ViewportPanel.h"
#include "instances/services/EngineService.h"
#include "instances/services/RenderingService.h"
#include "instances/services/RunService.h"
#include "instances/services/UIService.h"
#include "instances/services/user/InputService.h"
#include "instances/world/World.h"

using namespace Nyanners;
using namespace Nyanners::Services;

class TestApplication : public Application {
public:
	std::shared_ptr<Instances::Camera> camera;
	FrameBuffer *framebuffer = nullptr;
	Core::Rendering::Viewport* editorViewport;

	TestApplication() {
		m_Instance = this;
		this->init_rendering({1280, 720}, "TestApp");
		editorViewport = new Core::Rendering::Viewport();

		this->debugUI = ServiceProvider::instance()->add_service<DebugUIService>();
		this->camera = std::make_shared<Instances::Camera>();

		camera->name = "MainCamera";
		RenderingService::instance()->add_child(camera);
	}

	void start() override;
protected:
	void on_draw() const override;
	void on_update() override;

private:
	std::shared_ptr<DebugUIService> debugUI;
};

void TestApplication::start() {
	RenderingService::renderer->set_current_camera(camera);
	RenderingService::renderer->set_depth_test(Core::Rendering::Always);
	RenderingService::renderer->disable_depth_buffer();

	ScriptService::run_autorun();
	Application::start();
}

void TestApplication::on_draw() const {
	const auto world = currentModel->get_service<World>("World");
	const auto uiService = currentModel->get_service<UIService>("UIService");
	const auto renderService = RenderingService::instance();

	renderService->start_frame();

	// ^ start_frame might involve the user closing the window
	// therefore we stop here
	if (!renderService->is_window_open()) {
		return;
	}

	debugUI->draw_imgui();

	if (!renderService->active) {
		renderService->end_frame();
		return;
	}

	if (DebugUIService::renderWindows) {
		RenderingService::renderer->set_viewport(editorViewport);
		framebuffer->clear();
		RenderingService::renderer->render_from(world, nullptr, framebuffer);
		RenderingService::renderer->render_from(uiService, nullptr, framebuffer);
	} else {
		RenderingService::renderer->reset_viewport();
		RenderingService::renderer->render_from(world, nullptr, nullptr);
		RenderingService::renderer->render_from(uiService, nullptr, nullptr);
	}

	renderService->end_frame();
}

void TestApplication::on_update() {
	const auto runService = ServiceProvider::instance()->get_service<RunService>("RunService");


	if (has_rendering) {
		const auto renderService = RenderingService::instance();


		// TODO: Decouple event polling from RenderingService, somehow? maybe? if possible at all even
		while (const auto event = renderService->window->pollEvent()) {
			if (!event.has_value()) {
				continue;
			}

			// TODO: better way of doing this. idk
			auto *value = &event.value();

			EngineService::handle_event(value);
			renderService->handle_window_event(event);
			if (renderService->window->hasFocus()) {
				InputService::instance()->handle_event(value);
			}
		}
	}

	runService->tick();
}

int main() {
	auto *app = new TestApplication();
	const auto model = app->currentModel;
	const auto debugUI = app->currentModel->get_service<DebugUIService>("DebugUIService");

	app->framebuffer = new FrameBuffer(1280, 720);

	debugUI->add_child(std::make_shared<Debug::UI::ExplorerPanel>());
	debugUI->add_child(
	  std::make_shared<Debug::UI::ViewportPanel>(app->framebuffer, app->editorViewport)
	);
	debugUI->add_child(std::make_shared<Debug::UI::CommandBar>());
	debugUI->add_child(std::make_shared<Debug::UI::OutputPanel>());
	debugUI->add_child(std::make_shared<Debug::UI::MainMenubar>());

	Core::Logger::log("Running Test App");
	app->start();
	app->shutdown();

	delete app;
	return 0;
}
