#include "MainMenubar.h"
#include "Application.h"
#include "DebugUIService.h"
#include "imgui.h"
#include "core/Logger.h"
#include "instances/services/ReflectionService.h"
#include <algorithm>

using namespace Nyanners::Debug::UI;

void MainMenubar::draw() {
	const auto fps = renderService->fps;
	const auto frameTime = renderService->frameTime;

	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("Instances")) {
		if (ImGui::BeginMenu("Create..")) {
			for (const auto &descriptor :
			     ReflectionDescriptorRegistry::instance()->descriptors |
			       std::views::values) {

				if (descriptor.flags & 0) {
					continue;
				}

				if (ImGui::MenuItem(descriptor.name.c_str())) {
					const auto instance = descriptor.construct<Instances::Instance>();
					Application::instance()->currentModel->add_child(instance);
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

		ImGui::MenuItem("Debug Enabled", "F8", &Services::DebugUIService::renderWindows);

		if (ImGui::BeginMenu("Switch main camera...")) {
			for (auto& child : renderService->children) {
				if (child->baseName != "Camera") continue;
				auto camera = dynamic_pointer_cast<Instances::Camera>(child);
				auto isCurrentCamera = camera == Services::RenderingService::renderer->camera;

				if (ImGui::MenuItem(child->name.c_str(), "", &isCurrentCamera)) {
					Services::RenderingService::renderer->camera = camera;
				}
			}

			ImGui::EndMenu();
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

			if (ImGui::IsItemHovered()) {
				// bigger version for viewing
				ImGui::BeginTooltip();
					ImGui::Image(handle, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0));
					ImGui::Text("X: %d, Y: %d", texture->width, texture->height);
				ImGui::EndTooltip();
			}

			ImGui::Text(texture->debugIdentifier.c_str());

			ImGui::NextColumn();
		}

		ImGui::Columns(1);

		ImGui::End();
	}
}