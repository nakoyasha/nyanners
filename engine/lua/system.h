#pragma once
#include "../engine.h"
#include "iostream"
#include "luaImport.h"
#include "raylib.h"
#include "string"

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
    lua_getglobal(context, "_SUPER_SECRET_SCRIPT_NAME_I_SURE_HOPE_NO_ONE_SEES_THIS_AND_GETS_THE_SCRIPT_NAME_THIS_WAY");
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
    engine_panic(text);
    return 0;
}

int engine_LuaEngineExit(lua_State* context)
{
    engine_exit();
    return 0;
}

int engine_LuaDispatchMessage(lua_State* context)
{
    std::string message = luaL_checkstring(context, 1);

    if (message == "Engine.Exit") {
        std::cout << "Lua requested engine exit" << std::endl;
        engine_exit();
    } else {
        lua_throwError(context, "Invalid dispatch message");
    }

    return 0;
};