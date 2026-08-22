#include "NativeData.h"
#include "instances/services/ReflectionService.h"

using namespace Nyanners::Scripting::Reflection;

int NativeData::serialize(lua_State* context) {
	Services::ReflectionService::create_userdata<NativeData>(context, this, 0x09);
	const int dataIndex = lua_gettop(context);

	if (
		const int type = luaL_getmetatable(context, "NativeDataMeta"); type != LUA_TTABLE
	) {
		lua_pop(context, 1);
		if (luaL_newmetatable(context, "NativeDataMeta")) {
			const int metatable = lua_gettop(context);

			lua_pushcfunction(context, [](lua_State* context) -> int {
				auto* data = Services::ReflectionService::get_userdata_from_context<NativeData>(context, 1, 0x09);

				if (data == nullptr) {
					throw std::runtime_error("NativeData corruption... :(");
				}

				return data->lua_index(context);
				// return 0;
			}, "NativeData::_index");

			lua_setfield(context, metatable, "__index");

			lua_pushcfunction(context, [](lua_State* context) -> int {
				auto* data =
					Services::ReflectionService::get_userdata_from_context<NativeData>(
						context,
						1,
						0x09
					);

				if (data == nullptr) {
					throw std::runtime_error("NativeData corruption... :(");
				}

				// Capture NativeData in the iterator closure.
				lua_pushvalue(context, 1);
				lua_pushinteger(context, 0);

				lua_pushcclosure(
					context,
					[](lua_State* context) -> int {
						auto* data =
							Services::ReflectionService::get_userdata_from_context<NativeData>(
								context,
								lua_upvalueindex(1),
								0x09
							);

						if (data == nullptr) {
							throw std::runtime_error("closure corrupted");
						}

						return data->lua_iterate(context);
					},
					"NativeData::iterate_impl",
					2
				);

				return 1;
			}, "NativeData::__iter");
			lua_setfield(context, metatable, "__iter");

			lua_pushcfunction(context, [](lua_State* context) -> int {
				lua_pushstring(context, "NativeData");
				return 1;
			}, "NativeData:__tostring");
			lua_setfield(context, metatable, "__tostring");

			lua_setmetatable(context, dataIndex);
		} else {
			luaL_error(context, "Failed creating metatable for NativeData");
			return 0;
		}
	} else {
		lua_setmetatable(context, dataIndex);
	}

	return 1;
}

int NativeData::lua_index(lua_State *context) {
	if (lua_isnumber(context, -1)) {
		auto nativeId = lua_tonumber(context, -1);

		if (nativeId > static_cast<int>(children.size()) || nativeId < 0) {
			luaL_error(context, "NativeData OOB");
		}

		if (nativeId > 0) {
			nativeId -= 1;
		}

		const auto child = children[nativeId];
		push_child(context, child);
	}

	if (lua_isstring(context, -1)) {
		const auto fieldName = lua_tostring(context, -1);
		const auto field = fields.find(fieldName);

		if (field == fields.end()) {
			luaL_error(context, "Invalid NativeData property");
		}

		Services::ReflectionService::push_value(context, field->second);
		return 1;
	}

	luaL_error(context, "Attempt to index with invalid type");
	return 1;
}

int NativeData::lua_iterate(lua_State* context) {
	const auto index = lua_tointeger(context, lua_upvalueindex(2));

	if (index >= static_cast<lua_Integer>(children.size())) {
		return 0;
	}

	const auto child = children[index];

	lua_pushinteger(context, index + 1);
	lua_replace(context, lua_upvalueindex(2));
	lua_pushinteger(context, index + 1);
	push_child(context, child);

	return 2;
}

void NativeData::set_value(const std::string& keyName, const ReflectionValue& value) {
	this->fields[keyName] = value;
}

void NativeData::add_child(const NativeDataChild &child) {
	if (std::holds_alternative<NativeData*>(child) && std::get<NativeData*>(child) == this) {
		throw std::runtime_error("Attempt to create recursive NativeData");
	}

	this->children.push_back(child);
}

int NativeData::push_child(lua_State* context, const NativeDataChild &child) {
	if (std::holds_alternative<NativeData*>(child)) {
		const auto childData = std::get<NativeData*>(child);
		childData->serialize(context);
		return 1;
	}

	if (std::holds_alternative<ReflectionValue>(child)) {
		const auto value = std::get<ReflectionValue>(child);
		Services::ReflectionService::push_value(context, value);
		return 1;
	}

	return 0;
}
