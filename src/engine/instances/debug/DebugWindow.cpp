#include "DebugWindow.h"
#include "imgui.h"

using namespace Nyanners::Instances;

DebugWindow::DebugWindow() : Instance("DebugWindow") {
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  if (!ImGui_ImplOpenGL3_Init("#version 330")) {
    throw std::runtime_error("Failed to initialize ImGui");
  };
}

void DebugWindow::draw(sf::RenderTarget& target) {
  ImGuiIO& io = ImGui::GetIO();
  io.DisplaySize = ImVec2(
      static_cast<float>(target.getSize().x),
      static_cast<float>(target.getSize().y)
  );
  ImGui::NewFrame();
  ImGui_ImplOpenGL3_NewFrame();

  ImGui::Begin("test");
  ImGui::Text("if there's anything here i will be genuinely surprised");
  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugWindow::handle_event(const sf::RenderTarget& target, const sf::Event* event) {
  ImGuiIO& io = ImGui::GetIO();

  if (const auto* moved = event->getIf<sf::Event::MouseMoved>()) {
    if (moved->position.x > target.getSize().x || moved->position.x < 0) {
      return;
    }

    if (moved->position.y > target.getSize().y || moved->position.y < 0) {
      return;
    }

    io.MousePos = ImVec2(
      static_cast<float>(moved->position.x),
      static_cast<float>(moved->position.y)
    );
  } else if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
    if (mousePressed->button == sf::Mouse::Button::Left) {
      io.MouseDown[0] = true;
    } else if (mousePressed->button == sf::Mouse::Button::Right) {
      io.MouseDown[1] = true;
    } else if (mousePressed->button == sf::Mouse::Button::Middle) {
      io.MouseDown[2] = true;
    }
  } else if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
    if (mouseReleased->button == sf::Mouse::Button::Left) {
      io.MouseDown[0] = false;
    } else if (mouseReleased->button == sf::Mouse::Button::Right) {
      io.MouseDown[1] = false;
    } else if (mouseReleased->button == sf::Mouse::Button::Middle) {
      io.MouseDown[2] = false;
    }
  } else if (const auto* textEntered = event->getIf<sf::Event::TextEntered>()) {
    if (textEntered->unicode != 0x10000) {
      io.AddInputCharacter(static_cast<unsigned short>(textEntered->unicode));
    }
  } else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
    io.DisplaySize = ImVec2(
     static_cast<float>(resized->size.x),
     static_cast<float>(resized->size.y)
     );
  }
}