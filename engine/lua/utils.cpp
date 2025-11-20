#include "utils.h"

void lua_errorHandler(lua_State *context) {
    auto errorMessage = lua_tostring(context, -1);

  if (errorMessage == NULL) {
    // luaL_error(context, "Internal C++ Error");
    std::cout << "Internal LuaBridge error" << std::endl;
  } else {
    std::cout << errorMessage << std::endl;
  }

  //   luaL_traceback(context, context, NULL, 1);
  //   std::string stackTrace = lua_tostring(context, -1);

  //   std::cout << stackTrace << std::endl;
}