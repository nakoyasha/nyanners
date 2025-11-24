#pragma once

#include "instances/DataModel.h"
#include <optional>
#include <string>

void engine_panic(std::string message);
void engine_exit();

// #define _exit(...) exit

using namespace Nyanners::Instances;

class Application {
public:
    DataModel* dataModel;
    bool updatesPaused;

    double schedulerFpsCap = 60;
    double lastSchedulerCap = 0;
    bool headlessScreenshot = false;
    bool isRunning = false;
    int currentFPS = 0;

    char codeToEvaluate;

    Application(bool headlessMode = false)
    {
        headlessScreenshot = headlessMode;
    }

    static Application& instance(bool headlessMode = false)
    {
        static Application app(headlessMode);
        return app;
    }

    static Application* instancePointer()
    {
        Application* app;
        return app;
    }

    void panic(std::string message);
    void setFPS(double fpsCap);
    void setModel(DataModel* newModel);

    void start();
    void draw(std::optional<RenderTexture2D> texture);
    void drawDebug();
    void update();
    void stop();
};