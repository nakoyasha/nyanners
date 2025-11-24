#include "stdlib.h"
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <string.h>

#include "imgui.h"
#include <raylib.h>

#include "rlImGui.h"
#include "rlgl.h"

#include "./http/json.hpp"
#include "./instances/Rectangle.h"
#include "./instances/Script.h"
#include "./instances/TextLabel.h"
#include "./project/ProjectLoader.h"
#include "tinyfiledialogs.h"

#include "engine.h"
#include <thread>

#define _exit(...) exit

void Application::panic(std::string message)
{
    std::cout << message.c_str() << std::endl;
    tinyfd_messageBox("≡(▔﹏▔)≡", message.c_str(), "OK", "error", 0);
    stop();
}

void Application::setFPS(double fpsCap)
{
    SetTargetFPS(fpsCap);
}

void Application::setModel(DataModel* newModel)
{
    DataModel* oldModel = this->dataModel;
    this->dataModel = newModel;

    if (oldModel != nullptr) {
        delete oldModel;
    }
}

void Application::draw(std::optional<RenderTexture2D> texture)
{
    bool shouldRenderToTexture = texture.has_value();

    if (shouldRenderToTexture) {
        BeginTextureMode(texture.value());
    } else {
        BeginDrawing();
    }

    ClearBackground(BLACK);
    dataModel->draw();
    drawDebug();

    if (shouldRenderToTexture) {
        EndTextureMode();
    } else {
        EndDrawing();
    }
}

void Application::drawDebug()
{
    rlImGuiBegin();

    bool gccWantsThis = false;
    bool sceneSwap = false;

    ImGui::Begin("Nyanners Debug", &gccWantsThis);
    ImGui::Text("FPS: %d", currentFPS);
    ImGui::Text("Instances: %d", dataModel->children.size());
    ImGui::InputDouble("FPS Lock", &schedulerFpsCap, 1.0, 0.0);
    if (ImGui::Button("Set")) {
        setFPS(schedulerFpsCap);
    }

    if (ImGui::Button("Switch Scene")) {
        if (sceneSwap == false) {
            this->setModel(loadProjectFile("system/autorun.json"));
            sceneSwap = true;
        } else {
            this->setModel(loadProjectFile("system/project.json"));
            sceneSwap = false;
        }
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
    rlImGuiEnd();
}

void Application::update()
{
    currentFPS = GetFPS();
    if (updatesPaused)
        return;

    dataModel->update();
}

void Application::start()
{
    SetTraceLogLevel(TraceLogLevel::LOG_ERROR);
    int windowFlags = FLAG_WINDOW_RESIZABLE;

    if (headlessScreenshot == true) {
        windowFlags |= FLAG_WINDOW_HIDDEN;
    }

    SetConfigFlags(windowFlags);
    InitWindow(1280, 720, "Engine");
    rlImGuiSetup(true);
    DataModel* project = loadProjectFile("system/autorun.json");

    this->setModel(project);
    isRunning = true;

    if (!headlessScreenshot) {
        while (!WindowShouldClose() && isRunning) {
            update();
            draw(std::nullopt);
        }
    } else {
        RenderTexture2D texture = LoadRenderTexture(1280, 720);
        // render multiple times, just in case

        for (auto i = 1; 16; i++) {
            if (i == 16) {
                break;
            }

            update();
            draw(texture);
        }

        Image image = LoadImageFromTexture(texture.texture);
        ImageFlipVertical(&image);
        ExportImage(image, "render.png");
        UnloadImage(image);
        UnloadRenderTexture(texture);
    }
}

void Application::stop()
{
    this->isRunning = false;
    delete this->dataModel;
    this->dataModel = nullptr;
}

void engine_main()
{
    Application::instance(false).start();
}

void headless_main()
{
    Application::instance(true).start();
}

int main(int argc, char** argv)
{
    // if (argc >= 2) {
    //     std::string runType = argv[1];
    //     std::string runOutput = argv[2];

    //     if (runType == "headless") {
    //         headless_main();
    //         return 0;
    //     }

    //     return 0;
    // } else {
    //     engine_main();
    //     return 0;
    // }

    engine_main();
    return 0;
}
