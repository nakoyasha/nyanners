#pragma once
#include "glm/vec2.hpp"
#include "instances/Instance.h"
#include "instances/drawable/Drawable.h"

namespace Nyanners::Instances {
    class UIElement : virtual public Instance, public Drawable {
    public:
        UIElement() : Instance("UIElement") {};

        int zIndex = 0;
        glm::vec2 anchorPoint {};
        glm::vec2 sizeScale {};
        glm::vec2 sizeOffset {};
        glm::vec2 positionScale {};
        glm::vec2 positionOffset {};

        [[nodiscard]] int get_zindex() const;
        void set_zindex(int newZindex);
        [[nodiscard]] glm::vec2 get_anchor_point() const;
        void set_anchor_point(const glm::vec2 &newAnchorPoint);

        [[nodiscard]] glm::vec2 get_positionScale() const;
        void set_positionScale(const glm::vec2 &newSizeScale);
        [[nodiscard]] glm::vec2 get_positionOffset() const;
        void set_positionOffset(const glm::vec2 &newSizeOffset);

        [[nodiscard]] glm::vec2 get_sizeScale() const;
        void set_sizeScale(const glm::vec2 &newSizeScale);
        [[nodiscard]] glm::vec2 get_sizeOffset() const;
        void set_sizeOffset(const glm::vec2 &newSizeOffset);

        [[nodiscard]] glm::vec2 get_absolute_size() const;
        [[nodiscard]] glm::vec2 get_absolute_position() const;
    };
}
