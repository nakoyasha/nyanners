#include "engine.h"
#include "libInstance.h"
#include "lua/reflection/Reflection.h"

#include "instances/ui/TextLabel.h"
#include "instances/ui/ImageLabel.h"
#include "instances/ui/Button.h"
#include "instances/Script.h"
#include "instances/Sound.h"

#include "instances/containers/Model.h"
#include "instances/containers/Folder.h"

void libInstance_open(lua_State *context) {
    reflection_luaPushStruct(context, {
        {"new", &libInstance_new},
    });
    lua_setglobal(context, "Instance");
}

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
    } else if (type == "Script") {
        instanceCreated = new Script;
    } else if (type == "Sound") {
        instanceCreated = new SoundInstance;
    } else if (type == "Model") {
        instanceCreated = new Nyanners::Instances::Model;
    } else if (type == "Folder") {
        instanceCreated = new Folder;
    }
    else {
        lua_throwError(context, "Attempt to create invalid Instance");
        return 0;
    }

    reflection_exposeInstanceToLua(context, instanceCreated);
    return 1;
}