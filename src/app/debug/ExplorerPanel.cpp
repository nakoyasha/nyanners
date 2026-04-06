#include "ExplorerPanel.h"
#include "Application.h"
#include "imgui.h"

using namespace TestApp::Panels;

ExplorerPanel::ExplorerPanel() : Instance("ExplorerPanel") {
	activeDm = Nyanners::Application::instance()->currentModel;
	selectionService = activeDm->get_service<Nyanners::Services::SelectionService>("SelectionService");

	script = std::make_shared<Nyanners::Instances::Script>();
	script->initialize_script();
}

void ExplorerPanel::render_instance(const std::shared_ptr<Instance>& instance) {
	const ImGuiStyle& style = ImGui::GetStyle();

	ImGui::PushID(instance.get());
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;

	if (instance->children.empty()) {
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	bool isOpened = ImGui::TreeNodeEx(instance->name.c_str(), flags);


	if (ImGui::IsItemClicked()) {
		selectionService->set_selection(instance);
	}

	if (isOpened) {
		for (const auto& child : instance->children) {
			render_instance(child);
		}

		if (!instance->children.empty()) {
			ImGui::TreePop();
		}
	}

	if (instance->baseName == "World") {
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - style.IndentSpacing * 0.6f + 1.5f);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.f); // not the faintest idea

		ImGui::ImageWithBg(
			workspaceIcon->get_texture_handle(),
			ImVec2(16.f, 16.f),
			ImVec2(0.f, 0.f),
			ImVec2(1.f, 1.f),
			ImVec4(),
			instance->active ? ImVec4(1.f, 1.f, 1.f, 1.f) : ImVec4(.5f, .5f, .5f, 1.f)
		);
	}


	ImGui::PopID();
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
			// if (property.name == "Parent") {
			// 	continue;
			// }

			try {
				property.get(selectionService->currentSelection.get(), script->context);

				int type = lua_type(script->context, -1);

				if (type == LUA_TNUMBER) {
					const double value = lua_tonumber(script->context, -1);
					ImGui::Text(property.name.c_str());
					ImGui::SameLine();
					ImGui::Text(std::to_string(value).c_str());
				} else if (type == LUA_TSTRING) {
					const std::string value = lua_tostring(script->context, -1);
					ImGui::Text(property.name.c_str());
					ImGui::SameLine();
					ImGui::Text(value.c_str());
				} else if (type == LUA_TBOOLEAN) {
					bool value = lua_toboolean(script->context, -1);
					ImGui::Checkbox(property.name.c_str(), &value);
				} else if (type == LUA_TUSERDATA) {
					const auto* instance = Nyanners::Services::ReflectionService::get_instance_from_context(script->context, -1);


					ImGui::Text(property.name.c_str());
					ImGui::SameLine();
					if (instance == nullptr || instance->pointer == nullptr) {
						ImGui::Text("None");
					} else {
						ImGui::Text(instance->pointer->name.c_str());
					}
				}
				else {
					ImGui::Text(property.name.c_str());
					ImGui::SameLine();
					ImGui::Text("idk lol");
				}

				lua_pop(script->context, -1);
			} catch (void* exception) {
				ImGui::Text("idk lol");
			}
		}
	} else {
		ImGui::Text("No Instance selected");
	}

	ImGui::End();
}