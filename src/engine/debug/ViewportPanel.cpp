#include "ViewportPanel.h"
#include "Application.h"
#include "imgui.h"
#include "core/Logger.h"
#include "instances/services/RenderingService.h"

using namespace Nyanners::Debug::UI;

ViewportPanel::ViewportPanel(Resources::FrameBuffer* newFrameBuffer, Core::Rendering::Viewport* newViewport) : Instance("ViewportPanel") {
	framebuffer = newFrameBuffer;
	viewport = newViewport;
}

void ViewportPanel::draw() {
	const auto& frameBufferSize = framebuffer->size;
	ImGui::Begin(std::format("Viewport ({}, {})###Viewport", frameBufferSize.x, frameBufferSize.y).c_str());

	const auto window_width = std::max(1.0f, ImGui::GetContentRegionAvail().x);
	const auto window_height = std::max(1.0f, ImGui::GetContentRegionAvail().y);
	const auto& windowPosition = ImGui::GetCursorScreenPos();

	if (framebuffer != Services::RenderingService::renderer->defaultFramebuffer) {
		framebuffer->resize(static_cast<int>(window_width), static_cast<int>(window_height));
	}

	viewport->size = {
		static_cast<int>(window_width),
		static_cast<int>(window_height)
	};
	viewport->position = {
		static_cast<int>(windowPosition.x),
		static_cast<int>(windowPosition.y)
	};

	ImGui::GetWindowDrawList()->AddImage(
		framebuffer->get_texture_id(),
		ImVec2(windowPosition.x, windowPosition.y),
		ImVec2(windowPosition.x + window_width, windowPosition.y + window_height),
		ImVec2(0, 1),
		ImVec2(1, 0)
	);

	ImGui::End();
}

void ViewportPanel::update(const float deltaTime) {
	DebugWindow::update(deltaTime);
}