#include "ViewportPanel.h"
#include "Application.h"
#include "imgui.h"

using namespace TestApp::Panels;

ViewportPanel::ViewportPanel() : Instance("ViewportPanel") {
	renderService = Nyanners::Application::instance()->currentModel->get_service<Nyanners::Services::RenderingService>("RenderingService");
}

void TestApp::Panels::ViewportPanel::draw(const sf::RenderTarget& target) {
	ImGui::Begin("Viewport");

	const float window_width = ImGui::GetContentRegionAvail().x;
	const float window_height = ImGui::GetContentRegionAvail().y;

	renderService->framebuffer->resize(window_width, window_height);
	glViewport(0, 0, window_width, window_height);

	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImGui::GetWindowDrawList()->AddImage(
		(void*)renderService->framebuffer->get_texture_id(),
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