#include "ViewportPanel.h"
#include "Application.h"
#include "imgui.h"
#include "core/Logger.h"

using namespace Nyanners::Debug::UI;

ViewportPanel::ViewportPanel(Resources::FrameBuffer* newFrameBuffer, Core::Rendering::Viewport* newViewport) : Instance("ViewportPanel") {
	framebuffer = newFrameBuffer;
	viewport = newViewport;
}

void ViewportPanel::draw() {
	const auto& frameBufferSize = framebuffer->size;
	ImGui::Begin(std::format("Viewport ({}, {})###Viewport", frameBufferSize.x, frameBufferSize.y).c_str());

	const float window_width = ImGui::GetContentRegionAvail().x;
	const float window_height = ImGui::GetContentRegionAvail().y;

	const auto& windowPosition = ImGui::GetCursorScreenPos();

	glViewport(0, 0, window_width, window_height);
	framebuffer->resize(window_width, window_height);
	viewport->size = {window_width, window_height};
	viewport->position = {windowPosition.x, windowPosition.y};

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