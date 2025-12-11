#include "TextLabel.h"
#include "raylib.h"

using namespace Nyanners::Instances;

void TextLabel::draw()
{
    if (!m_visible) return;
    DrawText(text.c_str(), position.x, position.y, 24, WHITE);
}

int TextLabel::luaIndex(lua_State* context, std::string property)
{
    if (property == "Text") {
        lua_pushlstring(context, this->text.c_str(), this->text.size());
        return 1;
    } else {
        return UIDrawable::luaIndex(context, property);
    }
}

int TextLabel::luaNewIndex(lua_State* context, const std::string keyName, const std::string keyValue)
{
    if (keyName == "Text") {
        this->text = keyValue;
        return 1;
    } else {
        return Instance::luaNewIndex(context, keyName, keyValue);
    }
}