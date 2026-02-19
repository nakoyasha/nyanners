//
// Created by Haruka on 19.02.2026.
//

#pragma once
#include "lua.h"

void libImmediate_open(lua_State* context);
int libImmediate_start(lua_State* context);
int libImmediate_text(lua_State* context);
int libImmediate_inputText(lua_State* context);
int libImmediate_button(lua_State* context);
int libImmediate_combo(lua_State* context);

int libImmediate_getWindowSize(lua_State* context);
int libImmediate_setWindowSize(lua_State* context);
int libImmediate_getWindowPosition(lua_State* context);
int libImmediate_setWindowPosition(lua_State* context);
int libImmediate_end(lua_State* context);
