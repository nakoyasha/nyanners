#include "engine.h"
#include "libInstance.h"

int libInstance_new(lua_State* context)
{
    std::string type = luaL_checkstring(context, -1);

    if (type == "TextLabel") {
        TextLabel* label = new TextLabel;
        reflection_exposeInstanceToLua(context, label);
        // parent it to the datamodel
        Application::instance().dataModel->addChild(label);
        return 1;
    } else {
        lua_throwError(context, "Attempt to create invalid Instance");
        return 0;
    }
}