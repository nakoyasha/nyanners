#pragma once
#include "LayerCollector.h"

namespace Nyanners::Instances::UI {
    class Frame : public LayerCollector {
    public:
        Frame();

        void draw() override;
    };
}
