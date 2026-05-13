#pragma once
#include "DebugWindow.h"
#include "instances/services/RenderingService.h"

namespace Nyanners::Debug::UI {
	class ViewportPanel : public Instances::DebugWindow {
	public:
		ViewportPanel(Nyanners::Resources::FrameBuffer* newFrameBuffer);
		void draw() override;
		void update(const float deltaTime) override;
	private:
		std::shared_ptr<Nyanners::Services::RenderingService> renderService;
		Nyanners::Resources::FrameBuffer* framebuffer;
	};
} // TestApp