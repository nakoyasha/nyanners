#pragma once

#include "Instance.h"
#include "raylib.h"

namespace Nyanners {
namespace Instances {
    class Rectangle : public Instance {
    private:
        bool isReversed = false;

    public:
        int positionX = 0;
        int positionY = 90;

        double width = 32;
        double height = 32;

        Rectangle()
            : Instance("Rectangle")
        {
            this->m_className = "Rectangle";
            this->m_name = "Rectangle";
        }

        void update()
        {
            positionX += 24;

            if (isReversed == true) {
                height -= 1;
                width -= 1;

            } else {
                height += 1;
                width += 1;
            }

            if (positionX >= 600) {
                positionX = 0;
            };

            if (height >= 120) {
                isReversed = true;
            };

            if (width >= 120) {
                isReversed = true;
            }

            if (width == 32 && height == 32) {
                isReversed = false;
            }

            DrawRectangle(positionX, positionY, width, height, WHITE);
        }
    };
}
}