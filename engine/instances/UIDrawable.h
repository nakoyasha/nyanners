#pragma once

#include "Instance.h"
#include "raylib.h"

namespace Nyanners {
namespace Instances {
    class UIDrawable : public Instance {
    public:
        int zIndex = 0;
        Vector2 position {0,0};

        virtual bool isUI() {
            return true;
        }

        UIDrawable(std::string className) : Instance(className)
        { 
            this->m_className = className;
            this->m_name = "Instance";
        }
    };
}
}