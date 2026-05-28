#pragma once

#include "UIComponent.h"
#include "instances/Instance.h"

namespace Nyanners::UI::Components {
    class UIAspectRatioComponent : public Instances::Instance, public UIComponent {
    public:
        double targetAspectRatio = 1.0f;

        UIAspectRatioComponent() : Instance("UIAspectRatioComponent") {};


        void transform_scale(glm::vec2& scale);
        double get_aspect_ratio() const;
        void set_aspect_ratio(const double newAspectRatio);
    };
}