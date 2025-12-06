#include "engine.h"
#include "libInstance.h"
#include "lua/reflection/Reflection.h"

#include "instances/ui/TextLabel.h"
#include "instances/ui/ImageLabel.h"
#include "instances/ui/Button.h"

int libInstance_new(lua_State* context)
{
    std::string type = luaL_checkstring(context, -1);
    Instance* instanceCreated;

    if (type == "TextLabel") {
        instanceCreated = new TextLabel;
    } else if (type == "ImageLabel") {
        instanceCreated = new ImageLabel;
    } else if (type == "Button") {
        instanceCreated = new Button;
    }
    else {
        lua_throwError(context, "Attempt to create invalid Instance");
        return 0;
    }

    reflection_exposeInstanceToLua(context, instanceCreated);
    return 1;
}