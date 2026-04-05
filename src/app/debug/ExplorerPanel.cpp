#include "ExplorerPanel.h"
#include "Application.h"
#include "imgui.h"

using namespace TestApp::Panels;

ExplorerPanel::ExplorerPanel() : Instance("ExplorerPanel") {
	activeDm = Nyanners::Application::instance()->currentModel;
	selectionService = activeDm->get_service<Nyanners::Services::SelectionService>("SelectionService");
}

void ExplorerPanel::render_instance(const std::shared_ptr<Instance>& instance) {
	const ImGuiStyle& style = ImGui::GetStyle();

	if (ImGui::TreeNode(instance->name.c_str())) {
		for (const auto& child : instance->children) {
			render_instance(child);
		}

		ImGui::TreePop();
	}

	if (instance->baseName == "World") {
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - style.IndentSpacing * 0.6f + 1.5f);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.f); // not the faintest idea


		ImGui::ImageWithBg(
			workspaceIcon.textureId,
			ImVec2(16.f, 16.f),
			ImVec2(0.f, 0.f),
			ImVec2(1.f, 1.f),
			ImVec4(),
			instance->active ? ImVec4(1.f, 1.f, 1.f, 1.f) : ImVec4(.5f, .5f, .5f, 1.f)
		);
	}

	if (ImGui::IsItemClicked()) {
		selectionService->set_selection(instance);
	}
}

void ExplorerPanel::draw(const sf::RenderTarget& target) {
	ImGui::Begin("Explorer");
	ImGui::SetNextItemOpen(true);
	render_instance(activeDm);
	ImGui::End();

	ImGui::Begin("Properties");

	if (selectionService->currentSelection != nullptr) {
		static char searchBuffer[1024];
		ImGui::InputText("Search for properties", searchBuffer, IM_ARRAYSIZE(searchBuffer));
		const auto properties = Nyanners::Services::ReflectionService::get_properties(selectionService->currentSelection);

		for (const auto& property : properties) {
			ImGui::Text(property.name.c_str());
		}
	} else {
		ImGui::Text("No Instance selected");
	}

	ImGui::End();
}