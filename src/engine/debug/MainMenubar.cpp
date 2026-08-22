#include "Application.h"
#include "DebugUIService.h"
#include "MainMenubar.h"
#include "imgui.h"
#include "core/Logger.h"
#include "instances/services/ReflectionService.h"
#include "instances/services/SelectionService.h"
#include "scripting/reflections/ReflectionDescriptorRegistry.h"
#include "utils/ImGuiColor3.h"

using namespace Nyanners::Debug::UI;
using namespace Nyanners::Debug;

MainMenubar::MainMenubar() : Instance("MenubarPanel") {
	debugMenuIcon =
	  Resources::Texture::create(Texture2D, "assets/textures/editor/bug.png");
}

static void push_credit(
  const std::string &projectName,
  const std::string &creator,
  const std::string &link
) {
	if (ImGui::TextLink(projectName.c_str())) {
		Nyanners::Services::EngineService::open_url(link);
	}
	ImGui::SameLine();
	ImGui::TextColored(
	  color3_to_imvec4({0, 0, 0, 125}), "by %s", creator.c_str()
	);
}

void MainMenubar::draw() {
	const auto app = Application::instance();
	const auto renderService = app->renderService;
	const auto fps = renderService->fps;
	const auto frameTime = renderService->frameTime;
	auto selectionService =
	  Application::instance()
	    ->currentModel->get_service<Services::SelectionService>(
	      "SelectionService"
	    );

	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("Instances")) {
		if (ImGui::BeginMenu("Create..")) {
			for (const auto &descriptor :
			     ReflectionDescriptorRegistry::instance()->descriptors |
			       std::views::values) {

				if (
				  descriptor.flags &
				  static_cast<uint8_t>(ReflectionInstanceFlags::Service)
				) {
					continue;
				}

				if (
				  descriptor.flags &
				  static_cast<uint8_t>(ReflectionInstanceFlags::NotCreatable)
				) {
					continue;
				}

				if (ImGui::MenuItem(descriptor.name.c_str())) {
					const auto instance = descriptor.construct();
					if (
					  const auto child = std::dynamic_pointer_cast<Instance>(instance)
					) {
						selectionService->currentSelection->add_child(child);
					}
				}
			};
			ImGui::EndMenu();
		}

		if (selectionService->currentSelection != nullptr) {
			auto descriptor =
			  ReflectionDescriptorRegistry::instance()->descriptors.find(
			    selectionService->currentSelection->baseName
			  );
			const bool canClone =
			  descriptor != ReflectionDescriptorRegistry::instance()->descriptors.end() &&
			  (descriptor->second.flags &
			     static_cast<uint8_t>(ReflectionInstanceFlags::Service)) == 0 &&
			  (descriptor->second.flags &
			     static_cast<uint8_t>(ReflectionInstanceFlags::NotCreatable)) == 0;
			if (!canClone) ImGui::BeginDisabled();
			if (ImGui::Button("Perform Mitosis")) {
				auto selection = selectionService->currentSelection;
				// try {
				auto instance = selection->clone();
				selection->parent.lock()->add_child(instance);
				// } catch (const std::exception &error) {
					// Core::Logger::log_error(
					  // std::format("Unable to clone {}: {}", selection->name, error.what())
					// );
				// }
			}
			if (!canClone) ImGui::EndDisabled();
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("View")) {
		if (ImGui::MenuItem("Textures")) {
			showTextureViewer = !showTextureViewer;
		}

		ImGui::MenuItem(
		  "Debug Enabled", "F8", &Services::DebugUIService::renderWindows
		);

		if (ImGui::BeginMenu("Switch main camera...")) {
			for (auto &child : renderService->children) {
				if (child->baseName != "Camera") continue;
				auto camera = dynamic_pointer_cast<Instances::Camera>(child);
				auto isCurrentCamera =
				  camera == Services::RenderingService::renderer->camera;

				if (ImGui::MenuItem(child->name.c_str(), "", &isCurrentCamera)) {
					Services::RenderingService::renderer->camera = camera;
				}
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Help")) {
		if (ImGui::MenuItem("About Nyanners")) {
			// ImGui::OpenPopup("About");
			showAboutWindow = true;
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("##Debug")) {
		if (ImGui::MenuItem("Switch DM")) {
			const auto newModel = Application::make_datamodel();
			newModel->name = "TempDM";
			app->set_datamodel(newModel);
		}

		ImGui::EndMenu();
	}

	ImGui::Image(
	  debugMenuIcon->get_texture_handle(),
	  ImVec2(16, 16),
	  ImVec2(0, 1),
	  ImVec2(1, 0)
	);

	ImGui::SameLine(ImGui::GetWindowWidth() - 180.0f);
	ImGui::Text("%d FPS (%fms)", fps, frameTime);
	ImGui::EndMainMenuBar();

	if (showTextureViewer == true) {
		ImGui::Begin("Texture Viewer");

		ImGui::Columns(4, "texture_grid", false);
		for (const auto &texture : Nyanners::Services::RenderingService::textures) {
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

	if (showAboutWindow) {
		ImGui::OpenPopup("About Nyanners");
	}

	ImGui::SetNextWindowPos(
	  ImGui::GetMainViewport()->GetCenter(),
	  ImGuiCond_Appearing,
	  ImVec2(0.5f, 0.5f)
	);
	if (
	  ImGui::BeginPopupModal(
	    "About Nyanners", nullptr, ImGuiWindowFlags_AlwaysAutoResize
	  )
	) {
		ImGui::Text(
		  "Open-source Roblox-like game engine.\nThis software utilizes the following libraries:"
		);

		push_credit(
		  "Luau", "Roblox Corporation and contributors", "https://luau.org"
		);
		push_credit("Dear ImGui", "ocornut", "https://github.com/ocornut/imgui");
		push_credit("miniaudio", "mackron", "https://github.com/mackron/miniaudio");
		push_credit("FreeType", "Multiple contributors", "https://freetype.org");
		push_credit("SFML", "SFML Contributors", "https://freetype.org");

		ImGui::Text(
		  "This software is licensed under the GPLv3 license. Please consult"
		);
		ImGui::SameLine();
		if (ImGui::TextLink("the license")) {
			Services::EngineService::open_url(
			  "https://www.gnu.org/licenses/gpl-3.0.en.html"
			);
		}
		ImGui::SameLine();
		ImGui::Text(
		  "prior to usage, especially when making commerical derivatives of the software."
		);

		if (ImGui::Button("Close")) {
			showAboutWindow = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	};
}
