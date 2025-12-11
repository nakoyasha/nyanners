#pragma once

#include "instances/Instance.h"
#include "datatypes/UDim2.h"

#include "lua/reflection/Reflection.h"
#include "core/Logger.h"

namespace Nyanners {
namespace Instances {
    class UIDrawable : public Instance {
    public:
        int zIndex = 0;
        // Vector2 position {0,0};
        // Vector2 size {64, 64};

        DataTypes::UDim2 position;
        DataTypes::UDim2 size;

        bool m_visible = true;

        virtual bool isUI() {
            return true;
        }

        virtual ~UIDrawable() override = default;

        void setPosition(const float x, const float y) {
            position.setX(x);
            position.setY(y);

            renderingRectangle.x = position.absoluteX;
            renderingRectangle.y = position.absoluteY;
        }

        void setSize(const float x, const float y) {
            size.setX(x);
            size.setY(y);

            renderingRectangle.width = size.absoluteX;
            renderingRectangle.height = size.absoluteY;
        }

        int luaIndex(lua_State* context, const std::string keyName) override
        {
            if (keyName == "Size") {
                reflection_luaPushStruct(context, {
                    {"X", LuaValue(size.absoluteX)},
                    {"Y", LuaValue(size.absoluteY)}
                });
                return 1;
            } else if (keyName == "Position") {
                reflection_luaPushStruct(context, {
                    {"X", LuaValue(position.absoluteX)},
                    {"Y", LuaValue(position.absoluteY)}
                });
                return 1;
            }

            return Instance::luaIndex(context, keyName);
        }

        int luaNewIndex(lua_State* context, const std::string keyName, Vector2 keyValue) override
        {
            if (keyName == "Position") {
                this->setPosition(keyValue.x, keyValue.y);
                return 0;
            } else if (keyName == "Size") {
                this->setSize(keyValue.x, keyValue.y);
                return 0;
            }
            else {
                return Instance::luaNewIndex(context, keyName, keyValue);
            }
        }

        void update() {
            position.recomputeSize();
            size.recomputeSize();

            renderingRectangle.width = size.absoluteX;
            renderingRectangle.height = size.absoluteY;
        };

        int luaNewIndex(lua_State *context, std::string keyName, bool keyValue) override {
            if (keyName == "Visible") {
                this->m_visible = keyValue;
                return 0;
            }

            return Instance::luaNewIndex(context, keyName, keyValue);
        };

        UIDrawable(std::string className) : Instance(className)
        { 
            this->m_className = className;
            this->m_name = "Instance";
        }
    protected:
        Rectangle renderingRectangle = {position.absoluteX, position.absoluteY, size.absoluteX, size.absoluteY};
    };
}
}