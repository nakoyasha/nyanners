#pragma once
#include "DebugWindow.h"
#include "instances/services/RenderingService.h"

namespace Nyanners::Debug::UI {
	class ViewportPanel : public Instances::DebugWindow {
	public:
		ViewportPanel(Resources::FrameBuffer* newFrameBuffer, Core::Rendering::Viewport* newViewport);
		void draw() override;
		void update(float deltaTime) override;
	private:
		std::shared_ptr<Services::RenderingService> renderService;
		Resources::FrameBuffer* framebuffer;
		Core::Rendering::Viewport* viewport;
	};
} // TestApp