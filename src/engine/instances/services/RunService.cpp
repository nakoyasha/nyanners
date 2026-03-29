#include "RunService.h"
#include "RenderingService.h"
#include "UIService.h"

using namespace Nyanners::Services;

void RunService::bind_model(const std::shared_ptr<Instances::DataModel> newModel)
{
    this->model = newModel;
}

void RunService::run()
{
    clock.start();
    this->isRunning = true;

    // tickThread = std::thread([this]()
    // {
    const auto renderService = this->model->get_service<RenderingService>("RenderingService");
    const auto uiService = this->model->get_service<UIService>("UIService");

    while (this->isRunning == true && renderService->is_window_open())
    {
        this->tick();
        this->preRender->fire(deltaTime);
        renderService->render(uiService);
    }
    // });
}

void RunService::stop()
{
    clock.stop();
    this->isRunning = false;
    // this->tickThread.join();
}

void RunService::tick()
{
    const auto time = clock.restart();
    deltaTime = time.asSeconds();

    model->update(deltaTime);
}
