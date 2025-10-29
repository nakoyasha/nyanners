#include "system.h"

void lua_throwError(lua_State* context, std::string error)
{
    lua_pushstring(context, error.c_str());
    lua_error(context);
}

void luabridge_defineBridgeMethod(lua_State* context, std::string name,
    lua_CFunction method)
{
    lua_pushcfunction(context, method, name.c_str());
    lua_setglobal(context, name.c_str());
};

int luabridge_receiveMessageFromLua(lua_State* context)
{
    int argumentCount = lua_gettop(context);
    lua_getglobal(context, SCRIPT_NAME_GLOBAL);
    std::string scriptPath = lua_tostring(context, argumentCount + 1);

    for (int index = 1; index <= argumentCount; index++) {
        if (lua_isstring(context, index)) {
            std::string argument = lua_tostring(context, index);
            std::cout << "[Lua::" + scriptPath + "] " << argument << std::endl;
        } else {
            continue;
        }
    }

    return 0;
}

// char *lua_checkstring(lua_State *context, int stackIdx) {
//   if (lua_isstring(context, stackIdx)) {
//     lua_throwError(context, "did not receive a string");
//     return nullptr;
//   }

//   const char *string = lua_tostring(context, stackIdx);
// }

int engine_LuaDrawText(lua_State* context)
{
    std::string text = luaL_checkstring(context, 1);
    DrawText(text.c_str(), 190, 200, 20, LIGHTGRAY);
    return 0;
}

int engine_LuaEnginePanic(lua_State* context)
{
    std::string text = luaL_checkstring(context, 1);
    lua_getglobal(context, SCRIPT_NAME_GLOBAL);
    std::string scriptName = lua_tostring(context, 2);

    Application::instance().panic(std::string("Panic encountered while executing ") + scriptName + ":\n" + text);
    return 0;
}

int engine_LuaEngineExit(lua_State* context)
{
    Application::instance().exit();
    return 0;
}

Instance* reflection_getInstance(lua_State* context) {
    Instance* instance = *(Instance**)(lua_touserdata(context, 1));
    return instance;
}

int reflection_proxyInstanceMethod(lua_State* context, void (Instance::*)()) {
    method();
    return 0;
}

int reflection_metaIndex(lua_State* context) {
    Instance* instance = reflection_getInstance(context);
    std::string property = lua_tostring(context, 2);

    if (property == "Name") {
        lua_pushstring(context, instance->m_name.c_str());
        return 1;
    } else if (property == "ClassName") {
        lua_pushstring(context, instance->m_className.c_str());
    } else if (property == "Parent") {
        Instance** self = (Instance**)lua_newuserdata(context, sizeof(Instance*));
        *self = instance->m_parent;

        if (luaL_newmetatable(context, "lua_instance")) {
            luaL_Reg sRegs[] =
                {
                    { "__index", &reflection_metaIndex },
                    { nullptr, nullptr }
                };
            luaL_register(context, nullptr, sRegs);
        }

        lua_setmetatable(context, -2);
        return 1;
    } else if (property == "setFPS") {
        // Application app = Application::instance();
        // reflection_proxyInstanceMethod(context, &app);
    }
    else {
        lua_throwError(context, std::string("Attempt to index invalid property " + property).c_str());
        return 0;
    }

    return 0;
}

void reflection_createInstanceMetatable(lua_State* context) {
     if (luaL_newmetatable(context, "lua_instance")) {
        luaL_Reg sRegs[] =
            {
                { "__index", &reflection_metaIndex },
                { nullptr, nullptr }
            };
        luaL_register(context, nullptr, sRegs);
    }
}


void reflection_exposeInstanceToLua(lua_State* context, Instance* instance)
{
    Instance** self = (Instance**)lua_newuserdata(context, sizeof(Instance*));
    *self = instance;

    reflection_createInstanceMetatable(context);

    lua_setmetatable(context, -2);
};

static void lua_dumpstack (lua_State *L) {
  int top=lua_gettop(L);
  for (int i=1; i <= top; i++) {
    printf("%d\t%s\t", i, luaL_typename(L,i));
    switch (lua_type(L, i)) {
      case LUA_TNUMBER:
        printf("%g\n",lua_tonumber(L,i));
        break;
      case LUA_TSTRING:
        printf("%s\n",lua_tostring(L,i));
        break;
      case LUA_TBOOLEAN:
        printf("%s\n", (lua_toboolean(L, i) ? "true" : "false"));
        break;
      case LUA_TNIL:
        printf("%s\n", "nil");
        break;
      default:
        printf("%p\n",lua_topointer(L,i));
        break;
    }
  }
}

int engine_LuaDispatchMessage(lua_State* context)
{
    std::string message = luaL_checkstring(context, 1);

    if (message == "Engine.Exit") {
        std::cout << "Lua requested engine exit" << std::endl;
        Application::instance().exit();
    } else {
        lua_throwError(context, "Invalid dispatch message");
    }

    return 0;
};