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

        std::shared_ptr<Nyanners::Instances::Signal<float>> preRender;
        std::shared_ptr<Nyanners::Instances::Signal<float>> onTick;
        std::shared_ptr<Nyanners::Instances::Signal<>> onStop;

        RunService() : Instance("RunService")
        {
          preRender = std::make_shared<Instances::Signal<float>>();
          onTick = std::make_shared<Instances::Signal<float>>();
          onStop = std::make_shared<Instances::Signal<>>();
        };

        void bind_model(std::shared_ptr<Instances::DataModel> model);
        float get_time_since_start();
        void run();
        void stop();;
        void tick();

    private:
        std::shared_ptr<Instances::DataModel> model;
        std::thread tickThread;
        sf::Clock frameClock;
        sf::Clock startClock;
    };
}
