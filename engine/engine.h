#pragma once

#include <string>
#include "instances/DataModel.h"

void engine_panic(std::string message);
void engine_exit();

class Application {
public:
    Nyanners::Instances::DataModel* dataModel = new Nyanners::Instances::DataModel;
    bool updatesPaused;

    double schedulerFpsCap = 60;
    double lastSchedulerCap = 0;
    int currentFPS = 0;

    char codeToEvaluate;

    static Application& instance() {
        static Application app;
        return app;
    }

    static Application* instancePointer() {
        Application* app;
        return app;
    }

    void panic(std::string message);
    void setFPS(double fpsCap);

    void start();
    void draw();
    void update();
    void exit() {
        // _exit(1);
    }
};