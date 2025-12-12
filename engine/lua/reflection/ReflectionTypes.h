#pragma once
#include "string"
#include <functional>
#include <map>
#include <variant>

#include "lua.h"

using ReflectionMethod = std::function<int(lua_State*)>;
using LuaValue = std::variant<std::string, int, double, void*, lua_CFunction>;
using LuaStructMap = std::map<std::string, LuaValue>;

namespace Nyanners::Reflection {
	enum ReflectionPropertyType {
		Bool,
		String,
		Number,
		Float,
		Vector,
		Instance,
	};

	struct ReflectionProperty {
		const ReflectionPropertyType type;
		void* value;
	};

	using ReflectionProperties = std::map<std::string, ReflectionProperty>;
	using ReflectionMethods = std::map<std::string, ReflectionMethod>;
}