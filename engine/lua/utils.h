#pragma once
#include "iostream"
#include "luaImport.h"
#include "string"

void lua_errorHandler(lua_State *context);

// void luaBridge_callFunction(lua_State *context, std::string name) {
//   lua_State *newThread = lua_newthread(context);
//   lua_getglobal(context, "engine");
//   lua_getfield(context, -1, name.c_str());

//   lua_pushvalue(context, -1);       // push function to coroutine
//   lua_xmove(context, newThread, 1); // move function to coroutine stack
//   lua_pop(context, 1);

//   int result = lua_resume(newThread, context, 0);

//   printf("%d", result);

//   if (result != LUA_OK) {
//     lua_errorHandler(newThread);
//   } else if (result == LUA_YIELD) {
//     lua_throwError(context, "Illegal yield detected");
//   }
// }

// void luabridge_callEngineUpdate(lua_State *context) {
//   luaBridge_callFunction(context, "_internalUpdate");
// }