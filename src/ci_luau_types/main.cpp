#include "Application.h"
#include "instances/services/ScriptService.h"

class TypeBuilderApplication : public Nyanners::Application {
public:
	TypeBuilderApplication() {
		m_Instance = this;
	}
	void start() override;
};

void TypeBuilderApplication::start() {
	Nyanners::Services::ScriptService::run_autorun();
	Application::start();
	Application::shutdown();
}

int main() {
	auto *app = new TypeBuilderApplication();

	app->start();
	app->shutdown();
}
