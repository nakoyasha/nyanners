#include "RunService.h"
#include "ReflectionService.h"
#include "instances/debug/DebugUIService.h"

using namespace Nyanners::Services;

void RunService::bind_model(const std::shared_ptr<Instances::DataModel> newModel)
{
    this->model = newModel;
}

float RunService::get_time_since_start() {
  const auto time = this->startClock.getElapsedTime();
  return time.asSeconds();
}

void RunService::run()
{
    frameClock.start();
    this->isRunning = true;

    // tickThread = std::thread([this]()
    // {
  //   const auto renderService = this->model->get_service<RenderingService>("RenderingService");
  //   const auto uiService = this->model->get_service<UIService>("UIService");
  //   const auto world = this->model->get_service<World>("World");
		// const auto debugUI = this->model->get_service<DebugUIService>("DebugUIService");
  //
  //   while (this->isRunning == true && renderService->is_window_open())
  //   {
  //   		startClock.reset();
  //   		startClock.start();
  //
  //       this->tick();
  //       this->preRender->fire(deltaTime);
  //       renderService->start_frame();
  //
  //   		// ^ start_frame might involve the user closing the window
  //   		// therefore we stop here
  //   		if (!renderService->is_window_open()) {
  //   			break;
  //   		}
  //
  //       renderService->render(uiService);
  //       renderService->render(world);
  //   		debugUI->draw_imgui(renderService->window);
  //
  //       renderService->end_frame();
  //   }
    // });
}

void RunService::stop()
{
    frameClock.stop();
    startClock.reset();
    startClock.stop();
    this->isRunning = false;
    this->onStop->fire();
    // this->tickThread.join();
}

void RunService::tick()
{
    const auto time = frameClock.restart();
    deltaTime = time.asSeconds();

		startClock.reset();
		startClock.start();

		this->preRender->fire(deltaTime);
		// renderService->start_frame();

		// ^ start_frame might involve the user closing the window
		// therefore we stop here
		// if (!renderService->is_window_open()) {
			// break;
		// }

		// renderService->render(uiService);
		// renderService->render(world);
		// debugUI->draw_imgui(renderService->window);

		// renderService->end_frame();

    model->update(deltaTime);
}
