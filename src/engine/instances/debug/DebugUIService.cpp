#include "DebugUIService.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"

using namespace Nyanners::Services;

DebugUIService::DebugUIService() : Instance("DebugUIService") {
}

DebugUIService::~DebugUIService() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();
}

void DebugUIService::draw_imgui(const sf::Window* target) {
	ImGuiIO &io = ImGui::GetIO();
	io.DisplaySize = ImVec2(
	  static_cast<float>(target->getSize().x),
	  static_cast<float>(target->getSize().y)
	);
	ImGui::NewFrame();
	ImGui_ImplOpenGL3_NewFrame();


	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
	for (const auto &child : children) {
		const auto drawable = std::dynamic_pointer_cast<Instances::Drawable>(child);

		if (drawable == nullptr) {
			continue;
		}

		drawable->draw();
	}
}

void DebugUIService::on_frame_end() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugUIService::handle_event(
  const sf::Window* window, const sf::Event *event
) {
	ImGuiIO &io = ImGui::GetIO();

	if (const auto *moved = event->getIf<sf::Event::MouseMoved>()) {
		if (moved->position.x > window->getSize().x || moved->position.x < 0) {
			return;
		}

		if (moved->position.y > window->getSize().y || moved->position.y < 0) {
			return;
		}

		io.MousePos = ImVec2(
		  static_cast<float>(moved->position.x),
		  static_cast<float>(moved->position.y)
		);
	} else if (const auto *mousePressed =
	             event->getIf<sf::Event::MouseButtonPressed>()) {
		if (mousePressed->button == sf::Mouse::Button::Left) {
			io.MouseDown[0] = true;
		} else if (mousePressed->button == sf::Mouse::Button::Right) {
			io.MouseDown[1] = true;
		} else if (mousePressed->button == sf::Mouse::Button::Middle) {
			io.MouseDown[2] = true;
		}
	} else if (const auto *mouseReleased =
	             event->getIf<sf::Event::MouseButtonReleased>()) {
		if (mouseReleased->button == sf::Mouse::Button::Left) {
			io.MouseDown[0] = false;
		} else if (mouseReleased->button == sf::Mouse::Button::Right) {
			io.MouseDown[1] = false;
		} else if (mouseReleased->button == sf::Mouse::Button::Middle) {
			io.MouseDown[2] = false;
		}
	} else if (const auto *textEntered = event->getIf<sf::Event::TextEntered>()) {
		if (textEntered->unicode != 0x10000) {
			io.AddInputCharacter(static_cast<unsigned short>(textEntered->unicode));
		}
	} else if (const auto *resized = event->getIf<sf::Event::Resized>()) {
		io.DisplaySize = ImVec2(
		  static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)
		);
	}
}