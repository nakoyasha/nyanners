#pragma once

#include "UIDrawable.h"
#include "raylib.h"

namespace Nyanners {
namespace Instances {
    class TextLabel : public UIDrawable {
    public:
        std::string text = "TextLabel";
        int textSize = 24;
        void draw();
        int luaIndex(lua_State* context, const std::string property);
        int luaNewIndex(lua_State* context, const std::string keyName, const std::string keyValue);

        TextLabel()
            : UIDrawable("TextLabel")
        {
            this->m_name = "TextLabel";
        };
    };
}
}