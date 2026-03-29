#pragma once
#include <thread>

#include "instances/DataModel.h"
#include "instances/Instance.h"
#include "instances/basic/Signal.h"
#include "SFML/System/Clock.hpp"
#include "SFML/System/Time.hpp"

namespace Nyanners::Services
{
    class RunService : public Instances::Instance
    {
    public:
        float timeScale = 1.0f;
        float deltaTime = 1.0f;
        bool isRunning = false;

        Nyanners::Instances::Signal<float> preRender;
        Nyanners::Instances::Signal<float> onTick;

        RunService() : Instance("RunService")
        {
        };

        void bind_model(std::shared_ptr<Instances::DataModel> model);
        void run();
        void stop();;
        void tick();

    private:
        std::shared_ptr<Instances::DataModel> model;
        std::thread tickThread;
        sf::Clock clock;
    };
}
