#pragma once
#include "../instances/ui/TextLabel.h"
#include "instances/services/RenderingService.h"

namespace Nyanners::Debug {
    class FrameCounter : public Instances::TextLabel {
    public:
        FrameCounter();
        void draw() override;
    };
}
