#pragma once
#include "UIDrawable.h"
#include "instances/signal/Signal.h"

namespace Nyanners::Instances {
    class Button : public UIDrawable {
    public:
        std::string m_text = "Button";
        bool m_active = false;
        Signal<Vector2>* clicked;

        Button() : UIDrawable("Button")
        {
            clicked = new Signal<Vector2>();
        };

        ~Button() override;

        void draw() override;
        void update() override;
    };
}