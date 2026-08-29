#include "Application.h"
#include "core/Logger.h"
#include "debug/DebugUIService.h"
#include "debug/ExplorerPanel.h"
#include "instances/services/AssetService.h"
#include "instances/services/EngineService.h"
#include "instances/services/RenderingService.h"
#include "instances/services/RunService.h"
#include "instances/services/user/InputService.h"

using namespace Nyanners;
using namespace Nyanners::Services;

class TestApplication : public Application {
public:
	std::shared_ptr<Camera> camera;

	TestApplication() {
		m_Instance = this;
		this->init_rendering({1280, 720}, "TestApp");
		AssetService::instance()->set_asset_root("examples/ccraft/");
		this->camera = std::make_shared<Camera>();

		camera->name = "MainCamera";
		RenderingService::instance()->add_child(camera);
	}

	void start() override;
};

void TestApplication::start() {
	RenderingService::renderer->set_current_camera(camera);
	RenderingService::renderer->set_depth_test(Core::Rendering::Always);
	RenderingService::renderer->disable_depth_buffer();

	ScriptService::run_autorun();
	Application::start();
}

int main() {
	auto *app = new TestApplication();

	app->start();
	app->shutdown();

	delete app;
	return 0;
}
