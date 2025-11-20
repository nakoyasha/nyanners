#pragma once

#include "engine.h"
#include "iostream"
#include "raylib.h"
#include "string"

#include "lua.h"
#include "luacode.h"
#include "lualib.h"
#include <functional>
#include <variant>

static const char* SCRIPT_NAME_GLOBAL = "_SUPER_SECRET_SCRIPT_NAME_I_SURE_HOPE_NO_ONE_SEES_THIS_AND_GETS_THE_SCRIPT_NAME_THIS_WAY";
using namespace Nyanners::Instances;

void lua_throwError(lua_State* context, std::string error);
void luabridge_defineBridgeMethod(lua_State* context, std::string name,
    lua_CFunction method);
int luabridge_receiveMessageFromLua(lua_State* context);
static void luabridge_dumpstack(lua_State* L);

int engine_LuaDrawText(lua_State* context);

int engine_LuaEnginePanic(lua_State* context);
int engine_LuaEngineExit(lua_State* context);
int engine_LuaDispatchMessage(lua_State* context);

int reflection_instanceIsA(lua_State* context, Instance* instance);
Instance* reflection_getInstance(lua_State* context);
int reflection_metaIndex(lua_State* context);
void reflection_createInstanceMetatable(lua_State* context);
void reflection_exposeInstanceToLua(lua_State* context, Instance* instance);
void reflection_exposeInstanceToLuaShared(lua_State* context, std::shared_ptr<Instance> instance);

using ReflectionMethod = std::function<int(lua_State*)>;
using LuaValue = std::variant<std::string, int, double>;
using LuaStructMap = std::map<std::string, LuaValue>;

int reflection_luaPushMethod(lua_State* context, std::function<int(lua_State*)> method);
void reflection_luaPushStruct(lua_State* context, const std::map<std::string, LuaValue>& map);