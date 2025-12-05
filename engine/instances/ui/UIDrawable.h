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

        int luaNewIndex(lua_State* context, const std::string keyName, const std::string keyValue)
        {
            return Instance::luaNewIndex(context, keyName, keyValue);
        }

        int luaNewIndex(lua_State* context, const std::string keyName, Vector2 keyValue)
        {
            if (keyName == "Position") {
                this->position = keyValue;
                return 1;
            } else if (keyName == "Size") {
                this->size = keyValue;
            } else {
                return Instance::luaNewIndex(context, keyName, keyValue);
            }
        }

        UIDrawable(std::string className) : Instance(className)
        { 
            this->m_className = className;
            this->m_name = "Instance";
        }
    };
}
}