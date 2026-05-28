#pragma once
#include "UIElement.h"
#include "instances/Instance.h"
#include "instances/basic/Signal.h"

namespace Nyanners::Instances {
    class Button : public UIElement {
    public:
        Button();

        std::shared_ptr<Signal<>> onActivated;
        std::shared_ptr<Signal<>> onHold;
        std::shared_ptr<Signal<>> onRelease;

        void draw() override;
        void update(float deltaTime) override {}

        std::shared_ptr<SignalBase> get_activated() const;
        std::shared_ptr<SignalBase> get_hold() const;
        std::shared_ptr<SignalBase> get_release() const;
    private:
        bool isPressed = false;
    };
}
