#pragma once
#include "Application.h"
#include "DebugWindow.h"
#include "instances/services/RenderingService.h"

namespace Nyanners::Debug::UI {
	class MainMenubar : public Nyanners::Instances::DebugWindow {
	public:
		MainMenubar() : Instance("MenubarPanel") {
			renderService = Nyanners::Application::instance()->currentModel->get_service<Nyanners::Services::RenderingService>("RenderingService");
		};
		void draw() override;
	private:
		std::shared_ptr<Nyanners::Services::RenderingService> renderService;
		bool showTextureViewer = false;
	};
}