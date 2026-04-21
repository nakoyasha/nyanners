#include "DataTypes.h"
#include "core/Logger.h"
#include "third_party/luau/VM/src/ldo.h"

int Nyanners::Scripting::Reflection::push_color3(
  lua_State *context, DataTypes::Color3 &color
) {
	Services::ReflectionService::create_userdata(context, &color, 0x05);

	if (luaL_newmetatable(context, "color3")) {
		luaL_Reg sRegs[] = {
		  {"__index",
		   [](lua_State *context) {
			   auto instance = Services::ReflectionService::get_userdata_from_context<
			     DataTypes::Color3>(context, 1, 0x05);
			   const std::string propertyName = luaL_checkstring(context, -1);

			   if (propertyName == "r") {
				   lua_pushnumber(context, instance->r);
			   } else if (propertyName == "g") {
				   lua_pushnumber(context, instance->g);
			   } else if (propertyName == "b") {
				   lua_pushnumber(context, instance->b);
			   }

			   return 1;
		   }},
		  {"__newindex",
		   [](lua_State *context) {
			   luaL_error(
			     context,
			     "Create a new Color3 and set the Color property to it instead of modifiying raw values"
			   );
			   return 0;
		   }},
		  {
		    "__tostring",
		    [](lua_State *context) {
			    lua_pushstring(context, "Color3");
			    return 1;
		    },
		  },
		  {nullptr, nullptr}
		};

		luaL_register(context, nullptr, sRegs);
	}

	lua_setreadonly(context, -1, true);
	lua_setmetatable(context, -2);

	return 1;
}
int Nyanners::Scripting::Reflection::push_vector3(
  lua_State *context, glm::vec3 &vector
) {
	Services::ReflectionService::create_userdata(context, &vector, 0x06);

	if (luaL_newmetatable(context, "Vector3")) {
		luaL_Reg sRegs[] = {
		  {"__index",
		   [](lua_State *context) {
			   Core::Logger::log(
			     std::format("type at idx 1 {}", luaL_typename(context, 1))
			   );
			   Core::Logger::log(
			     std::format("type at idx 2 {}", luaL_typename(context, 2))
			   );

			   Core::Logger::log(
			     std::format("type at idx -1 {}", luaL_typename(context, -1))
			   );
			   Core::Logger::log(
			     std::format("type at idx -2 {}", luaL_typename(context, -2))
			   );
			   auto instance =
			     Services::ReflectionService::get_userdata_from_context<glm::vec3>(
			       context, 1, 0x06
			     );
			   const std::string propertyName = luaL_checkstring(context, -1);

			   if (propertyName == "x") {
				   lua_pushnumber(context, instance->x);
			   } else if (propertyName == "y") {
				   lua_pushnumber(context, instance->y);
			   } else if (propertyName == "z") {
				   lua_pushnumber(context, instance->z);
			   }

			   return 1;
		   }},
		  {"__newindex",
		   [](lua_State *context) {
			   luaL_error(
			     context,
			     "Create a new Vector3 and set the Position property to it instead of modifiying raw values"
			   );
			   return 0;
		   }},
		  {
		    "__tostring",
		    [](lua_State *context) {
			    lua_pushstring(context, "Vector3");
			    return 1;
		    },
		  },
		  {nullptr, nullptr}
		};

		luaL_register(context, nullptr, sRegs);
	}

	lua_setreadonly(context, -1, true);
	lua_setmetatable(context, -2);

	return 1;
}