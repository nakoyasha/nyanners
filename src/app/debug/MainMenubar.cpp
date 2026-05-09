#include "MainMenubar.h"
#include "Application.h"
#include "imgui.h"
#include "core/Logger.h"
#include "instances/services/ReflectionService.h"
#include <algorithm>

using namespace TestApp::Panels;

void MainMenubar::draw() {
	const auto fps = renderService->fps;
	const auto frameTime = renderService->frameTime;

	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("Instances")) {
		if (ImGui::BeginMenu("Create..")) {
			for (const auto &descriptor :
			     Nyanners::Services::ReflectionService::classes |
			       std::views::values) {

				if (!std::ranges::contains(
				      descriptor.flags,
				      Nyanners::Scripting::Reflection::ReflectionInstanceFlags::
				        Creatable
				    )) {
					continue;
				}

				if (ImGui::MenuItem(descriptor.className.c_str())) {
					const auto instance = descriptor.constructor();
					Nyanners::Application::instance()->currentModel->add_child(instance);
				}
			};
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("View")) {
		if (ImGui::MenuItem("Textures")) {
			showTextureViewer = !showTextureViewer;
		}

		ImGui::EndMenu();
	}

	ImGui::SameLine(ImGui::GetWindowWidth() - 180.0f);
	ImGui::Text("%d FPS", fps);
	ImGui::Text("%fms", frameTime);
	ImGui::EndMainMenuBar();

	if (showTextureViewer == true) {
		ImGui::Begin("Texture Viewer");

		ImGui::Columns(4, "texture_grid", false);
		for (const auto& texture : Nyanners::Services::RenderingService::textures) {
			const auto handle = texture->get_texture_handle();
			ImGui::Image(handle, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::Text(texture->debugIdentifier.c_str());

			ImGui::NextColumn();
		}

		ImGui::Columns(1);

		ImGui::End();
	}
}