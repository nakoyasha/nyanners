#pragma once
#include "imgui_impl_opengl3.h"
#include "SFML/Window/Event.hpp"
#include "instances/Instance.h"
#include "instances/basic/Signal.h"
#include "instances/drawable/Drawable.h"

namespace Nyanners::Instances {
    class DebugWindow : public Instance, public Drawable {
    public:
    	Signal<>* onImmediateRender;
      DebugWindow();

      void draw(const sf::RenderTarget &target) override;
      static void handle_event(const sf::RenderTarget& target, const sf::Event* event);

      ~DebugWindow() override {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();
      }
    private:
      double newFps = 60;
    };
}