#pragma once
#include "instances/debug/DebugWindow.h"
#include "instances/services/RenderingService.h"

namespace TestApp::Panels {
	class ViewportPanel : public Nyanners::Instances::DebugWindow {
	public:
		ViewportPanel(Nyanners::Resources::FrameBuffer* newFrameBuffer);
		void draw() override;
		void update(const float deltaTime) override;
	private:
		std::shared_ptr<Nyanners::Services::RenderingService> renderService;
		Nyanners::Resources::FrameBuffer* framebuffer;
	};
} // TestApp