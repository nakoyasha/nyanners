#include "DebugUIService.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "instances/services/RenderingService.h"

using namespace Nyanners::Services;

DebugUIService::DebugUIService() : Instance("DebugUIService") {}

DebugUIService::~DebugUIService() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();
}

void DebugUIService::draw_imgui() {
	ImGuiIO &io = ImGui::GetIO();
	const auto size = RenderingService::renderer->get_window_size();
	io.DisplaySize =
	  ImVec2(static_cast<float>(size.x), static_cast<float>(size.y));
	ImGui::NewFrame();
	ImGui_ImplOpenGL3_NewFrame();

	ImGui::DockSpaceOverViewport(
	  0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode
	);
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

static ImGuiKey toImGuiKey(sf::Keyboard::Key key) {
	switch (key) {
		case sf::Keyboard::Key::Tab:
			return ImGuiKey_Tab;
		case sf::Keyboard::Key::Left:
			return ImGuiKey_LeftArrow;
		case sf::Keyboard::Key::Right:
			return ImGuiKey_RightArrow;
		case sf::Keyboard::Key::Up:
			return ImGuiKey_UpArrow;
		case sf::Keyboard::Key::Down:
			return ImGuiKey_DownArrow;
		case sf::Keyboard::Key::PageUp:
			return ImGuiKey_PageUp;
		case sf::Keyboard::Key::PageDown:
			return ImGuiKey_PageDown;
		case sf::Keyboard::Key::Home:
			return ImGuiKey_Home;
		case sf::Keyboard::Key::End:
			return ImGuiKey_End;
		case sf::Keyboard::Key::Insert:
			return ImGuiKey_Insert;
		case sf::Keyboard::Key::Delete:
			return ImGuiKey_Delete;
		case sf::Keyboard::Key::Backspace:
			return ImGuiKey_Backspace;
		case sf::Keyboard::Key::Space:
			return ImGuiKey_Space;
		case sf::Keyboard::Key::Enter:
			return ImGuiKey_Enter;
		case sf::Keyboard::Key::Escape:
			return ImGuiKey_Escape;
		case sf::Keyboard::Key::A:
			return ImGuiKey_A;
		case sf::Keyboard::Key::C:
			return ImGuiKey_C;
		case sf::Keyboard::Key::V:
			return ImGuiKey_V;
		case sf::Keyboard::Key::X:
			return ImGuiKey_X;
		case sf::Keyboard::Key::Y:
			return ImGuiKey_Y;
		case sf::Keyboard::Key::Z:
			return ImGuiKey_Z;
		default:
			return ImGuiKey_None;
	}
}

void DebugUIService::handle_event(
  const sf::Window *window, const sf::Event *event
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
	} else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {

		ImGuiKey imguiKey = toImGuiKey(keyPressed->code);
		if (imguiKey != ImGuiKey_None) {
			io.AddKeyEvent(imguiKey, true);
		}

		// Modifiers
		io.AddKeyEvent(ImGuiKey_LeftCtrl, keyPressed->control);
		io.AddKeyEvent(ImGuiKey_LeftShift, keyPressed->shift);
		io.AddKeyEvent(ImGuiKey_LeftAlt, keyPressed->alt);
		io.AddKeyEvent(ImGuiKey_LeftSuper, keyPressed->system);

	} else if (const auto *keyReleased = event->getIf<sf::Event::KeyReleased>()) {

		ImGuiKey imguiKey = toImGuiKey(keyReleased->code);
		if (imguiKey != ImGuiKey_None) {
			io.AddKeyEvent(imguiKey, false);
		}

		// Modifiers
		io.AddKeyEvent(ImGuiKey_LeftCtrl, keyReleased->control);
		io.AddKeyEvent(ImGuiKey_LeftShift, keyReleased->shift);
		io.AddKeyEvent(ImGuiKey_LeftAlt, keyReleased->alt);
		io.AddKeyEvent(ImGuiKey_LeftSuper, keyReleased->system);
	}
}