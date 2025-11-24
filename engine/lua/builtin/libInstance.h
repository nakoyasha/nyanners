#pragma once
#include "lua.h"
#include "lua/system.h"
#include "luacode.h"
#include "lualib.h"

#include "instances/TextLabel.h"

using namespace Nyanners::Instances;

int libInstance_new(lua_State* context);