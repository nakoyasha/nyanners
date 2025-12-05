#include "engine.h"
#include "libInstance.h"
#include "lua/reflection/Reflection.h"

int libInstance_new(lua_State* context)
{
    std::string type = luaL_checkstring(context, -1);

    if (type == "TextLabel") {
        TextLabel* label = new TextLabel;
        // parent it to the datamodel
        // Application::instance().dataModel->addChild(label);
        reflection_exposeInstanceToLua(context, label);
        lua_gc(context, LUA_GCCOLLECT, 0);   // force collection
        return 1;
    } else {
        lua_throwError(context, "Attempt to create invalid Instance");
        return 0;
    }
}