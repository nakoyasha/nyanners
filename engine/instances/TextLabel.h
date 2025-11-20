#pragma once

#include "UIDrawable.h"
#include "raylib.h"

namespace Nyanners {
namespace Instances {
    class TextLabel : public UIDrawable {
    public:
        std::string text = "TextLabel";
        int textSize = 24;

        void draw() {
            DrawText(text.c_str(), position.x, position.y, 24, WHITE);
        }

        TextLabel() : UIDrawable("TextLabel") {
            this->m_name = "TextLabel";
        };
    };
}
}