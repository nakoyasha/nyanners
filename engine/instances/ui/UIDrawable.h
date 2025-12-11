#pragma once

#include "instances/Instance.h"
#include "datatypes/UDim2.h"

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

        bool isUI() override;
        UIDrawable(std::string className);
        virtual ~UIDrawable() override = default;

        void setPosition(const float x, const float y);
        void setSize(const float x, const float y);
        void setZIndex(const int zIndex);

        int luaIndex(lua_State* context, const std::string keyName) override;
        int luaNewIndex(lua_State* context, const std::string keyName, Vector2 keyValue) override;
        int luaNewIndex(lua_State *context, std::string keyName, float keyValue) override;
        int luaNewIndex(lua_State *context, std::string keyName, bool keyValue) override;

        void update();
    protected:
        Rectangle renderingRectangle = {position.absoluteX, position.absoluteY, size.absoluteX, size.absoluteY};
    };
}
}
