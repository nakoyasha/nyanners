#include "DataTypes.h"

#include "lualib.h"
#include "core/Logger.h"
#include "scripting/data/UserdataTags.h"
#include "third_party/luau/VM/src/ldo.h"

using namespace Nyanners::Services;

int Nyanners::Scripting::Reflection::push_color3(
  lua_State *context, DataTypes::Color3 &color
) {
	ReflectionService::create_userdata(context, &color, 0x05);

	if (luaL_newmetatable(context, "color3")) {
		luaL_Reg sRegs[] = {
		  {"__index",
		   [](lua_State *context) {
			   auto instance =
			     ReflectionService::get_userdata_from_context<DataTypes::Color3>(
			       context, 1, 0x05
			     );
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
	ReflectionService::create_userdata(context, &vector, 0x06);

	if (luaL_newmetatable(context, "Vector3")) {
		luaL_Reg sRegs[] = {
		  {"__index",
		   [](lua_State *context) {
			   auto instance =
			     ReflectionService::get_userdata_from_context<glm::vec3>(
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
		  {
		    "__add",
		    [](lua_State *context) {
			    auto aVector =
			      ReflectionService::get_userdata_from_context<glm::vec3>(
			        context, 1, 0x06
			      );
			    auto bVector =
			      ReflectionService::get_userdata_from_context<glm::vec3>(
			        context, 2, 0x06
			      );

		    	if (!bVector || !aVector) {
		    		throw std::logic_error("How the fuck are either of these missing");
		    	}

			    auto cVector = *aVector + *bVector;
			    push_vector3(context, cVector);

			    return 1;
		    },
		  },
		  {
		    "__sub",
		    [](lua_State *context) {
			    auto aVector =
			      ReflectionService::get_userdata_from_context<glm::vec3>(
			        context, 1, 0x06
			      );
			    auto bVector =
			      ReflectionService::get_userdata_from_context<glm::vec3>(
			        context, 2, 0x06
			      );

		    	if (!bVector || !aVector) {
		    		throw std::logic_error("How the fuck are either of these missing");
		    	}

			    auto cVector = *aVector - *bVector;
			    push_vector3(context, cVector);

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

int Nyanners::Scripting::Reflection::push_vector2(
  lua_State *context, glm::vec2 &vector
) {
	ReflectionService::create_userdata(context, &vector, 0x07);

	if (luaL_newmetatable(context, "Vector2")) {
		luaL_Reg sRegs[] = {
		  {"__index",
		   [](lua_State *context) {
			   auto instance =
			     ReflectionService::get_userdata_from_context<glm::vec2>(
			       context, 1, 0x07
			     );
			   const std::string propertyName = luaL_checkstring(context, -1);

			   if (propertyName == "x") {
				   lua_pushnumber(context, instance->x);
			   } else if (propertyName == "y") {
				   lua_pushnumber(context, instance->y);
			   }

			   return 1;
		   }},
		  {"__newindex",
		   [](lua_State *context) {
			   luaL_error(
			     context,
			     "Create a new Vector2 and set the Position property to it instead of modifiying raw values"
			   );
			   return 0;
		   }},
		  {
		    "__tostring",
		    [](lua_State *context) {
			    lua_pushstring(context, "Vector2");
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
glm::vec3 *Nyanners::Scripting::Reflection::get_vector3_from_lua(
  lua_State *context, int idx
) {
	auto *position =
	  ReflectionService::get_userdata_from_context<glm::vec3>(context, idx, 0x06);

	if (position == nullptr) {
		throw std::runtime_error("Vector3 is nullptr");
	}

	return position;
}

glm::vec2 *Nyanners::Scripting::Reflection::get_vector2_from_lua(
  lua_State *context, int idx
) {
	auto *position =
	  ReflectionService::get_userdata_from_context<glm::vec2>(context, idx, 0x07);

	if (position == nullptr) {
		throw std::runtime_error("Vector2 is nullptr");
	}

	return position;
}
Nyanners::DataTypes::Color3 *
Nyanners::Scripting::Reflection::get_color_from_lua(
  lua_State *context, int idx
) {
	auto *color = ReflectionService::get_userdata_from_context<DataTypes::Color3>(context, idx, 0x05);

	if (color == nullptr) {
		throw std::runtime_error("Color3 is nullptr");
	}

	return color;
}