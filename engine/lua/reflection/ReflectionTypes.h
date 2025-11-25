#pragma once
#include "string"
#include <functional>
#include <map>
#include <variant>

#include "lua.h"
#include "luacode.h"
#include "lualib.h"

using ReflectionMethod = std::function<int(lua_State*)>;
using LuaValue = std::variant<std::string, int, double>;
using LuaStructMap = std::map<std::string, LuaValue>;