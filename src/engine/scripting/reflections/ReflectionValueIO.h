#pragma once
#include "ReflectionPropertyReaders.h"
#include "lua.h"
#include "lualib.h"
#include <memory>
#include <string>
#include <type_traits>

#include "NativeData.h"

namespace Nyanners::Scripting::Reflection {
	std::shared_ptr<Instances::Object>
	get_object_from_lua(lua_State *context, int index);

	void push_object_to_lua(
	  lua_State *context, const std::shared_ptr<Instances::Object> &object
	);

	template <typename T> T read_argument(lua_State *context, int index);

	template <>
	inline lua_State *read_argument<lua_State *>(lua_State *context, const int) {
		return context;
	}

	template <>
	inline bool read_argument<bool>(lua_State *context, const int index) {
		return luaL_checkboolean(context, index);
	}

	template <>
	inline int read_argument<int>(lua_State *context, const int index) {
		return static_cast<int>(luaL_checknumber(context, index));
	}

	template <>
	inline float read_argument<float>(lua_State *context, const int index) {
		return static_cast<float>(luaL_checknumber(context, index));
	}

	template <>
	inline double read_argument<double>(lua_State *context, const int index) {
		return luaL_checknumber(context, index);
	}

	template <>
	inline std::string
	read_argument<std::string>(lua_State *context, const int index) {
		return luaL_checkstring(context, index);
	}

	template <typename T> struct shared_object_type;

	template <typename T> struct shared_object_type<std::shared_ptr<T>> {
		using type = T;
	};

	template <typename T>
	T read_argument(lua_State *context, const int index) {
		if constexpr (requires { typename shared_object_type<T>::type; }) {
			const auto object = get_object_from_lua(context, index);
			using ObjectType = typename shared_object_type<T>::type;
			static_assert(std::is_base_of_v<Instances::Object, ObjectType>);

			auto typedObject = std::dynamic_pointer_cast<ObjectType>(object);

			if (typedObject == nullptr) {
				luaL_error(context, "Lua object has an incompatible type");
			}

			return typedObject;
		} else {
			return read_value<T>(context, index);
		}
	}

	template <typename T> void push_value(lua_State *context, const T &value) {
		if constexpr (std::is_same_v<T, bool>) lua_pushboolean(context, value);
		else if constexpr (std::is_same_v<T, int>)
			lua_pushinteger(context, value);
		else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
			lua_pushnumber(context, value);
		else if constexpr (std::is_same_v<T, std::string>)
			lua_pushlstring(context, value.data(), value.size());
		else if constexpr (requires { typename shared_object_type<T>::type; })
			push_object_to_lua(context, value);
		else if constexpr (std::is_same_v<T, NativeData*>)
			static_cast<NativeData*>(value)->serialize(context);
		else
			lua_pushnil(context);
			// static_assert(
			  // std::is_same_v<T, void>, "Unsupported automatic Lua return type"
			// );
	}
}
