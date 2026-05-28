#include "RunService.h"
#include "debug/DebugUIService.h"

using namespace Nyanners::Services;

void RunService::bind_model(const std::shared_ptr<Instances::DataModel> newModel)
{
    this->model = newModel;
}

float RunService::get_time_since_start() const {
  const auto time = this->startClock.getElapsedTime();
  return time.asSeconds();
}

void RunService::run()
{
    frameClock.start();
    this->isRunning = true;
}

void RunService::stop()
{
    frameClock.stop();
    startClock.reset();
    startClock.stop();
    this->isRunning = false;
    this->onStop->fire();
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

std::shared_ptr<Nyanners::Instances::SignalBase> RunService::get_on_tick() const {
	return this->preRender;
}
