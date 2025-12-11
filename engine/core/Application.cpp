#include <fstream>
#include <iostream>

#include "imgui.h"
#include "imgui_stdlib.h"
#include <raylib.h>

#include "rlImGui.h"

#include "./http/json.hpp"
#include "./instances/Script.h"
#include "tinyfiledialogs.h"

#include "core/Logger.h"
#include "engine.h"

#define _exit(...) exit

std::unordered_set<Instance*> Application::allInstances;

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
    // there is a bug with raylib wheere we will
    // sometimes draw texture index 1
    // instead of our own. this looks really broken thus we pause rendering here up until the new datamodel is up and created
    renderingPaused = true;
    updatesPaused = true;

    DataModel* oldModel = this->dataModel;
    this->dataModel = newModel;

    if (oldModel != nullptr) {
        delete oldModel;
    }

    renderingPaused = false;
    updatesPaused = false;
}

void Application::draw(std::optional<RenderTexture2D> texture)
{
    if (renderingPaused) {
        BeginDrawing();
        // drawDebug();
        EndDrawing();
        return;
    }

    // we can't draw anything while the datamodel is null, so let's render
    // literally nothing
    if (dataModel == nullptr) {
        BeginDrawing();
        EndDrawing();
        return;
    }

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

void debug_renderInstanceChildren(Instance* instance, int padding = 0) {
    std::string repeat = std::string("");

    for (auto i = 0; i < padding;i++) {
        repeat += std::string("  ");
    }

    for (auto child : instance->children) {
        ImGui::Text((repeat + child->m_name + std::string("(") + child->m_className + std::string(")")).c_str());

        if (child->children.size() != 0) {
            debug_renderInstanceChildren(child, padding + 1);
        }
    }
}

void debug_renderProperties(Instance* instance) {

    if (ImGui::BeginTable("Properties", 2, ImGuiTableFlags_SizingStretchProp)) {
        for (auto const& prop : instance->properties) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            const std::string keyName = prop.first;
            const auto property = prop.second;

            ImGui::Text(keyName.c_str());

            ImGui::TableSetColumnIndex(1);
            switch (property.type) {
                case (Nyanners::Reflection::String): {
                    std::string* value = static_cast<std::string*>(property.value);
                    ImGui::InputText(keyName.c_str(), value, 1000);
                    break;
                }
                case (Nyanners::Reflection::ReflectionPropertyType::Number): {
                    float* value = static_cast<float*>(property.value);
                    ImGui::InputFloat(keyName.c_str(), value, 1);
                    break;
                }
                case (Nyanners::Reflection::Instance): {
                    Instance* instance = Nyanners::Reflection::getInstanceFromProperty(property);

                    if (instance != nullptr) {
                        ImGui::Text(instance->m_name.c_str());
                    } else {
                        ImGui::Text("None");
                    }
                }
                default: {
                    // ImGui::Text("meow");
                    break;
                }
            }

            ImGui::TableNextRow();
        }
        ImGui::EndTable();
    }

}

void Application::drawDebug()
{
    rlImGuiBegin();

    bool gccWantsThis = false;
    ImGui::Begin("Nyanners Debug", &gccWantsThis);
    ImGui::Text("FPS: %d", currentFPS);
    ImGui::Text("Instances: %d", dataModel->children.size());
    ImGui::InputDouble("FPS Lock", &schedulerFpsCap, 1.0, 0.0);
    if (ImGui::Button("Set")) {
        setFPS(schedulerFpsCap);
    }

    if (ImGui::Button("Switch Scene")) {
        if (sceneSwap == false) {
            this->setModel(new DataModel("system/autorun.json"));
            sceneSwap = true;
        } else {
            this->setModel(new DataModel("system/project.json"));
            sceneSwap = false;
        }
    }

    ImGui::Checkbox("Engine Paused", &updatesPaused);
    ImGui::Checkbox("Rendering Paused", &renderingPaused);

    ImGui::InputText("Lua Eval", &codeToEvaluate, 1024);
    if (ImGui::Button("Execute")) {
        Nyanners::Instances::Script* script = new Nyanners::Instances::Script;
        script->m_name = "<eval>";
        script->loadFromString(&codeToEvaluate);
        script->runScript();

        // delete after running
        delete script;
    }

    if (ImGui::Button("Force Crash")) {
        panic("hii :) forced crashed from drawing thread\n\n\n\n\n\n\n\n\n\n\n\n\n\nhehehe");
    };

    if (ImGui::CollapsingHeader("DataModel Children")) {
        debug_renderInstanceChildren(dataModel);
    }

    ImGui::End();

    ImGui::Begin("Properties");
    debug_renderProperties(this->dataModel);
    ImGui::End();



    rlImGuiEnd();
}

void Application::update()
{
    currentFPS = GetFPS();
    if (updatesPaused || renderingPaused || isRunning != true)
        return;

    dataModel->update();

    // int oldX  = screenSize.width;
    // int oldY = screenSize.height;

    screenSize.width = GetScreenWidth();
    screenSize.height = GetScreenHeight();

    // if (auto screenWidth = GetScreenWidth() != screenSize.width) {
    //     screenSize.width = screenWidth;
    //     onResize.fire(screenSize);
    // }
    //
    // if (auto screenHeight = GetScreenHeight() != screenSize.height) {
    //     screenSize.height = screenHeight;
    //     onResize.fire(screenSize);
    // }
}

void Application::start()
{
    InitAudioDevice();

    if (!IsAudioDeviceReady()) {
        panic("Audio device initialization failed");
        return;
    }

    Nyanners::Logger::log("Starting engine");
    SetTraceLogLevel(TraceLogLevel::LOG_ALL);
    int windowFlags = FLAG_WINDOW_RESIZABLE;

    if (headlessScreenshot == true) {
        Nyanners::Logger::log("Running in headless mode");
        windowFlags |= FLAG_WINDOW_HIDDEN;
    }

    SetConfigFlags(windowFlags);
    InitWindow(screenSize.width, screenSize.height, "Engine");
    rlImGuiSetup(true);
    DataModel* project = new DataModel("system/project.json");
    this->setModel(project);
    isRunning = true;

    if (!headlessScreenshot) {
        while (WindowShouldClose() == false && isRunning == true) {
            update();
            draw(std::nullopt);
        }
    } else {
        RenderTexture2D texture = LoadRenderTexture(screenSize.width, screenSize.height);
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
    CloseAudioDevice();
    this->isRunning = false;
    this->dataModel = nullptr;
    delete this->dataModel;
}

void Application::addInstance(Instance* instance) {
    allInstances.insert(instance);
}

void Application::removeInstance(Instance *instance) {
    allInstances.erase(instance);
}

bool Application::isInstanceValid(Instance *instance) {
    return allInstances.contains(instance);
}

