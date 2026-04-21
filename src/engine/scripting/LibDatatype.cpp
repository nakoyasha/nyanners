#include "LibDatatype.h"
#include "lua.h"
#include "lualib.h"
#include "reflections/DataTypes.h"

using namespace Nyanners::Scripting;

int luaVector3_new(lua_State* context) {
	const double x = luaL_checknumber(context, -3);
	const double y = luaL_checknumber(context, -2);
	const double z = luaL_checknumber(context, -1);
	const auto vector = new glm::vec3(x, y, z);

	Reflection::push_vector3(context, *vector);
	return 1;
}

void LibDatatype::attach(lua_State *context) {
	// Color3
	lua_createtable(context, 0, 1);
	lua_pushcfunction(context, [](lua_State* context) {
		const double r = luaL_checknumber(context, -4);
		const double g = luaL_checknumber(context, -3);
		const double b = luaL_checknumber(context, -2);
		const double alpha = luaL_checknumber(context, -1);

		const auto color = new DataTypes::Color3(r, g, b, alpha);

		Reflection::push_color3(context, *color);
		return 1;
	}, "Color3.new");
	lua_setfield(context, 2, "new");
	lua_setglobal(context, "Color3");
	lua_createtable(context, 0, 1);
	lua_pushcfunction(context, luaVector3_new, "Vector3.new");
	lua_setfield(context, 2, "new");
	lua_setglobal(context, "Vector3");
}