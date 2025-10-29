#include <raylib.h>
#include "stdlib.h"
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <string.h>

#include "rlImGui.h"
#include "rlgl.h"
#include "imgui.h"

#include "tinyfiledialogs.h"
#include "./instances/Rectangle.h"
#include "./instances/Script.h"
#include "./instances/TextLabel.h"

#include "engine.h"

#define _exit(...) exit

void Application::panic(std::string message) {
    std::cout << message.c_str() << std::endl;
    tinyfd_messageBox("Engine Failure", message.c_str(), "OK", "error", 0);
    exit();
}

void Application::draw() {
    currentFPS = GetFPS();
    BeginDrawing();
    ClearBackground(BLACK);
    dataModel.draw();
    rlImGuiBegin();

    ImGui::Begin("Nyanners Debug", false);
    ImGui::Text("FPS: %d", currentFPS);
    ImGui::Text("Instances: %d", dataModel.children.size());

    ImGui::InputDouble("FPS Lock", &schedulerFpsCap, 1.0, 0.0);
    ImGui::Checkbox("Engine Paused", &updatesPaused);

    if (ImGui::Button("Force Crash")) {
        panic("hii :) forced crashed from drawing thread\n\n\n\n\n\n\n\n\n\n\n\n\n\nhehehe");
    };

    if (ImGui::CollapsingHeader("DataModel Children")) {
        for (auto instance : dataModel.children) {
            ImGui::Text((instance->m_name + std::string("(") + instance->m_className + std::string(")")).c_str());
        }
    }

    ImGui::End();

    rlImGuiEnd();
    EndDrawing();
}

void Application::update() {
    if (schedulerFpsCap != lastSchedulerCap) {
        SetTargetFPS(schedulerFpsCap);
        lastSchedulerCap = schedulerFpsCap;
    }

    if (!updatesPaused) {
        dataModel.update();
    }
}

void Application::start() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "Engine");
    rlImGuiSetup(true);
    Nyanners::Instances::Script* script = new Nyanners::Instances::Script;
    Nyanners::Instances::TextLabel* label = new Nyanners::Instances::TextLabel;
    label->text = "hii";
    // label->position.x = 300;

    dataModel.addChild(new Nyanners::Instances::Rectangle);
    dataModel.addChild(label);
    dataModel.addChild(script);
    // script->loadFromFile("system/luau/autorun.luau");
    script->loadFromFile("D:/Development/nyanners/ceng/system/luau/autorun.luau");
    DisableCursor();

    while (!WindowShouldClose()) {
        draw();
        update();
    }

}

void engine_main()
{
    Application app = Application::instance();
    app.start();
}

int main() {
    engine_main();
     return 0;
}
