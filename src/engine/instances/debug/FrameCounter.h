#pragma once
#include "instances/drawable/TextLabel.h"
#include "instances/services/RenderingService.h"

namespace Nyanners::Debug {
    class FrameCounter : public Instances::TextLabel {
    public:
        FrameCounter();
        void update(const float deltaTime) override;
    private:
        std::shared_ptr<Services::RenderingService> renderService;
    };
}
