#pragma once
#include "lua.h"
#include "lualib.h"
#include "instances/datatypes/Color3.h"
#include <string>

namespace Nyanners::Scripting::Reflection {
	glm::vec3 *get_vector3_from_lua(lua_State *context, int idx);
	glm::vec2 *get_vector2_from_lua(lua_State *context, int idx);
	DataTypes::Color3 *get_color_from_lua(lua_State *context, int idx);

	template <typename T> T read_value(lua_State *context, int index);

	template <>
	inline bool read_value<bool>(lua_State *context, const int index) {
		return luaL_checkboolean(context, index);
	};

	template <>
	inline double read_value<double>(lua_State *context, const int index) {
		return luaL_checknumber(context, index);
	};

	template <> inline int read_value<int>(lua_State *context, const int index) {
		return static_cast<int>(luaL_checknumber(context, index));
	};

	template <>
	inline std::string
	read_value<std::string>(lua_State *context, const int index) {
		return luaL_checkstring(context, index);
	};

	template <>
	inline glm::vec3 read_value<glm::vec3>(lua_State *context, const int index) {
		return *get_vector3_from_lua(context, index);
	};

	template <>
	inline glm::vec2 read_value<glm::vec2>(lua_State *context, const int index) {
		return *get_vector2_from_lua(context, index);
	};

	template <>
	inline DataTypes::Color3
	read_value<DataTypes::Color3>(lua_State *context, const int index) {
		return *get_color_from_lua(context, index);
	};

	// for instances, use ReflectionService::get_instance_from_context
}