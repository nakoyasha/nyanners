#pragma once

#include "instances/Instance.h"
#include "instances/DataModel.h"
#include <optional>
#include <string>
#include <unordered_set>

void engine_panic(std::string message);
void engine_exit();

// #define _exit(...) exit

using namespace Nyanners::Instances;

struct Vector2Int {
    int width;
    int height;
};

class Application {
public:
    DataModel* dataModel;
    // This set serves to check at runtime whether an instance is truly a valid or not
    // I have no idea how else to do this.
    static std::unordered_set<Instance*> allInstances;
    bool updatesPaused;
    bool renderingPaused;

    double schedulerFpsCap = 60;
    double lastSchedulerCap = 0;
    bool headlessScreenshot = false;
    bool isRunning = false;
    bool sceneSwap = false;
    int currentFPS = 0;
    
    Vector2Int screenSize = {1280, 720};

    char codeToEvaluate;

    static void addInstance(Instance* instance);
    static void removeInstance(Instance* instance);
    static bool isInstanceValid(Instance* instance);

    static Application& instance()
    {
        static Application app;
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