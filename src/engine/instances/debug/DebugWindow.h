#pragma once
#include "imgui_impl_opengl3.h"
#include "SFML/Window/Event.hpp"
#include "instances/Instance.h"

namespace Nyanners::Instances {
    class DebugWindow : public Instance, public Drawable {
    public:
      DebugWindow();

      void draw(sf::RenderTarget& target) override;
      static void handle_event(const sf::RenderTarget& target, const sf::Event* event);

      ~DebugWindow() override {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();
      }
    };
}