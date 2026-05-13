//
// Created by nyako on 22.04.26.
//

#include "FrameCounter.h"
#include "Application.h"
#include "instances/services/RenderingService.h"

using namespace Nyanners::Debug;

FrameCounter::FrameCounter() : Instance("FrameCounter") {
    renderService = Application::instance()->currentModel->get_service<Services::RenderingService>("RenderingService");
}

void FrameCounter::draw() {
    const auto fps = renderService->fps;
    const auto windowSize = renderService->renderer->get_window_size();
    this->set_text(std::format("FPS: {}", fps));
    this->set_position({0, windowSize.y - 120, 0});

    TextLabel::draw();
}
