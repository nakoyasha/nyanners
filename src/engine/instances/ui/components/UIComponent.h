#pragma once
#include "glm/vec2.hpp"


namespace Nyanners::UI::Components {
    class UIComponent {
    public:
        virtual ~UIComponent() = default;

        virtual void transform_position(glm::vec2& position) {};
        virtual void transform_scale(glm::vec2& scale) {};
    };
}
