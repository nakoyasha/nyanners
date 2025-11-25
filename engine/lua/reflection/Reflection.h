#pragma once
#include "ReflectionTypes.h"
#include "instances/Instance.h"

int reflection_instanceIsA(lua_State* context, Nyanners::Instances::Instance* instance);
Nyanners::Instances::Instance* reflection_getInstance(lua_State* context);
int reflection_metaIndex(lua_State* context);
void reflection_createInstanceMetatable(lua_State* context);
void reflection_exposeInstanceToLua(lua_State* context, Nyanners::Instances::Instance* instance);

int reflection_luaPushMethod(lua_State* context, std::function<int(lua_State*)> method);
void reflection_luaPushStruct(lua_State* context, const std::map<std::string, LuaValue>& map);