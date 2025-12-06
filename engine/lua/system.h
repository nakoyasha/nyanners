#pragma once

#include "string"
#include <filesystem>

#include "lua.h"

static const char* SCRIPT_NAME_GLOBAL = "_SUPER_SECRET_SCRIPT_NAME_I_SURE_HOPE_NO_ONE_SEES_THIS_AND_GETS_THE_SCRIPT_NAME_THIS_WAY";

void lua_throwError(lua_State* context, std::string error);
void luabridge_defineBridgeMethod(lua_State* context, std::string name,
    lua_CFunction method);
int luabridge_receiveMessageFromLua(lua_State* context);
static void luabridge_dumpstack(lua_State* L);

int engine_LuaDrawText(lua_State* context);

int engine_LuaEnginePanic(lua_State* context);
int engine_LuaEngineExit(lua_State* context);
int engine_LuaDispatchMessage(lua_State* context);
std::string engine_readFile(std::string fileName);