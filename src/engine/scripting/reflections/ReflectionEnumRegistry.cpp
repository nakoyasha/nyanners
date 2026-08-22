#include "ReflectionEnumRegistry.h"

#include "instances/services/ReflectionService.h"

using namespace Nyanners::Scripting::Reflection;

void ReflectionEnumRegistry::register_values(const std::string &name, const std::map<std::string, int> &values) {
	enums[name] = values;
}

const std::map<std::string, int> * ReflectionEnumRegistry::find_values(const std::string &name) const {
	const auto values = enums.find(name);
	return values == enums.end() ? nullptr : &values->second;
}

void ReflectionEnumRegistry::push_to_lua(lua_State *context) const {
	lua_newtable(context);

	const int enumTable = lua_gettop(context);

	for (const auto &[enumName, values] : enums) {
		std::map<std::string, ReflectionValue> copy;

		for (const auto &[name, value] : values) {
			copy[name] = ReflectionValue {value};
		}

		Services::ReflectionService::push_struct(context, copy);

		lua_setfield(context, enumTable, enumName.c_str());
	}

	lua_setglobal(context, "Enum");
}
