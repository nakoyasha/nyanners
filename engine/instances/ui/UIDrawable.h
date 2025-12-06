#pragma once

#include "instances/Instance.h"

namespace Nyanners {
namespace Instances {
    class UIDrawable : public Instance {
    public:
        int zIndex = 0;
        Vector2 position {0,0};
        Vector2 size {64, 64};

        virtual bool isUI() {
            return true;
        }

        virtual ~UIDrawable() override = default;

        void setPosition(const Vector2 newPosition) {
            position = newPosition;
            renderingRectangle.x = position.x;
            renderingRectangle.y = position.y;
        }

        void setSize(const Vector2 newSize) {
            size = newSize;
            renderingRectangle.width = size.x;
            renderingRectangle.height = size.y;
        }

        int luaNewIndex(lua_State* context, const std::string keyName, const std::string keyValue) override
        {
            return Instance::luaNewIndex(context, keyName, keyValue);
        }

        int luaNewIndex(lua_State* context, const std::string keyName, Vector2 keyValue) override
        {
            if (keyName == "Position") {
                this->setPosition(keyValue);
                return 0;
            } else if (keyName == "Size") {
                this->setSize(keyValue);
                return 0;
            } else {
                return Instance::luaNewIndex(context, keyName, keyValue);
            }
        }

        UIDrawable(std::string className) : Instance(className)
        { 
            this->m_className = className;
            this->m_name = "Instance";
        }
    protected:
        Rectangle renderingRectangle = {position.x, position.y, size.x, size.y};
    };
}
}