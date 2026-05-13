#include "ViewportPanel.h"
#include "Application.h"
#include "imgui.h"
#include "core/Logger.h"

using namespace Nyanners::Debug::UI;

ViewportPanel::ViewportPanel(Nyanners::Resources::FrameBuffer* newFrameBuffer) : Instance("ViewportPanel") {
	renderService = Nyanners::Application::instance()->currentModel->get_service<Nyanners::Services::RenderingService>("RenderingService");
	framebuffer = newFrameBuffer;
}

void ViewportPanel::draw() {
	ImGui::Begin("Viewport");

	const float window_width = ImGui::GetContentRegionAvail().x;
	const float window_height = ImGui::GetContentRegionAvail().y;

	glViewport(0, 0, window_width, window_height);
	framebuffer->resize(window_width, window_height);

	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImGui::GetWindowDrawList()->AddImage(
		framebuffer->get_texture_id(),
		ImVec2(pos.x, pos.y),
		ImVec2(pos.x + window_width, pos.y + window_height),
		ImVec2(0, 1),
		ImVec2(1, 0)
);

	ImGui::End();
}

void ViewportPanel::update(const float deltaTime) {
	DebugWindow::update(deltaTime);
}