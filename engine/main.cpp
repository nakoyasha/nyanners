#include "stdlib.h"
#include <fstream>
#include <iostream>
#include <raylib.h>
#include <stdint.h>
#include <string.h>

#include "imgui.h"
#include "rlImGui.h"
#include "rlgl.h"

#include "./instances/Rectangle.h"
#include "./instances/Script.h"
#include "./instances/TextLabel.h"
#include "tinyfiledialogs.h"

#include "engine.h"
#include <thread>

#define _exit(...) exit

void Application::panic(std::string message)
{
    std::cout << message.c_str() << std::endl;
    tinyfd_messageBox("Engine Failure", message.c_str(), "OK", "error", 0);
    exit();
}

void Application::setFPS(double fpsCap)
{
    SetTargetFPS(fpsCap);
}

void Application::draw()
{
    currentFPS = GetFPS();
    BeginDrawing();
    ClearBackground(BLACK);
    dataModel->draw();
    rlImGuiBegin();

    bool gccWantsThis = false;

    ImGui::Begin("Nyanners Debug", &gccWantsThis);
    ImGui::Text("FPS: %d", currentFPS);
    ImGui::Text("Instances: %d", dataModel->children.size());
    ImGui::InputDouble("FPS Lock", &schedulerFpsCap, 1.0, 0.0);
    if (ImGui::Button("Set")) {
        setFPS(schedulerFpsCap);
    }

    ImGui::Checkbox("Engine Paused", &updatesPaused);

    ImGui::InputText("Lua Eval", &codeToEvaluate, 1024);
    if (ImGui::Button("Execute")) {
        Nyanners::Instances::Script* script = new Nyanners::Instances::Script;
        script->loadFromString(&codeToEvaluate);
    }

    if (ImGui::Button("Force Crash")) {
        panic("hii :) forced crashed from drawing thread\n\n\n\n\n\n\n\n\n\n\n\n\n\nhehehe");
    };

    if (ImGui::CollapsingHeader("DataModel Children")) {
        for (auto instance : dataModel->children) {
            ImGui::Text((instance->m_name + std::string("(") + instance->m_className + std::string(")")).c_str());
        }
    }

    ImGui::End();
    // ImGui::ShowDemoWindow();
    // ImGui::ShowDebugLogWindow();

    rlImGuiEnd();
    EndDrawing();
    // script->loadFromString("local a = 20");
}

void Application::update()
{
    if (updatesPaused) {
        return;
    }

    dataModel->update();
}

void Application::start()
{
    std::cout << std::filesystem::current_path() << std::endl;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Engine");
    rlImGuiSetup(true);
    Nyanners::Instances::Script* script = new Nyanners::Instances::Script;
    Nyanners::Instances::TextLabel* label = new Nyanners::Instances::TextLabel;
    label->text = "hii";
    label->position.x = 300;

    dataModel->addChild(new Nyanners::Instances::Rectangle);
    dataModel->addChild(label);
    dataModel->addChild(script);
    script->loadFromFile("./system/luau/autorun.luau");

    // for (int i = 1; i < 1000; i++) {
    //     Nyanners::Instances::Rectangle* rectangle = new Nyanners::Instances::Rectangle;
    //     dataModel->addChild(rectangle);
    // }

    // // script->loadFromFile("system/luau/autorun.luau");
    DisableCursor();
    while (!WindowShouldClose()) {
        update();
        draw();
    }
}

void engine_main()
{
    Application::instance().start();
}

int main()
{
    engine_main();
    return 0;
}
