#pragma once
#include <unordered_map>
#include <vector>

#include "ReflectionProperty.h"

namespace Nyanners::Scripting::Reflection {

	class NativeData {
	public:
		using NativeDataChild = std::variant<NativeData*, ReflectionValue>;
		std::unordered_map<std::string, ReflectionValue> fields;
		std::vector<NativeDataChild> children;

		int serialize(lua_State* context);
		int lua_index(lua_State* context);
		int lua_iterate(lua_State* context);
		void set_value(const std::string& keyName, const ReflectionValue& value);
		void add_child(const NativeDataChild &child);

	private:
		int push_child(lua_State* context, const NativeDataChild& child);
	};
}
