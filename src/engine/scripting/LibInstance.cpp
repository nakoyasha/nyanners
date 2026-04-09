#include "LibInstance.h"
#include "lualib.h"
#include "instances/services/ReflectionService.h"

using namespace Nyanners::Scripting;

void LibInstance::attach(lua_State *context) {
	lua_createtable(context, 0, 1);
	lua_pushcfunction(context, [](lua_State* context) {
		const std::string className = luaL_checkstring(context, -1);
		const auto instance = LibInstance::create_instance(className);

		if (instance == nullptr) {
			luaL_error(context, "Cannot create instance because no such instance exists");
			return 0;
		}

		Services::ReflectionService::reflect_class(context, instance);
		return 1;
	}, "Instance.new");
	lua_setfield(context, 2, "new");
}
std::shared_ptr<Nyanners::Instances::Instance>
LibInstance::create_instance(const std::string &className) {
	const auto classes = Services::ReflectionService::classes;

	for (const auto &iterator : classes) {
		const auto instanceClass = iterator.second;

		if (instanceClass.className == className) {
			return instanceClass.constructor();
		}
	}

	return nullptr;
}