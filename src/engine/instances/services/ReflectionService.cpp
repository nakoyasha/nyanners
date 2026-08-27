#include "Application.h"
#include "EngineService.h"
#include "ReflectionService.h"
#include "RenderingService.h"
#include "lualib.h"
#include "core/Logger.h"
#include "instances/basic/Signal.h"
#include "instances/container/BasicContainers.h"
#include "instances/drawable/MeshPart.h"
#include "instances/services/RunService.h"
#include "scripting/data/UserdataTags.h"
#include "scripting/reflections/DataTypes.h"
#include "scripting/reflections/ReflectionEnumRegistry.h"
#include "scripting/reflections/ReflectionPropertyReaders.h"
#include "scripting/reflections/ReflectionTypes.h"
#include "scripting/reflections/ReflectionValueIO.h"
#include <algorithm>

using namespace Nyanners::Services;
using namespace Nyanners::Scripting::Reflection;


namespace Nyanners::Scripting {
	static auto reflectionServiceDescriptor =
			ReflectionDescriptorRegistry::instance()->create_registrator([]() {
				ReflectionService::create_descriptor("ReflectionService", {"Instance"})
						.add_method<
							&ReflectionService::generate_lua_reflection_table>(
							"get_descriptors", Null, {}
						);
			});
}

void ReflectionService::register_enum(
	const std::string &enumName, const std::map<std::string, int> &values
) {
	ReflectionEnumRegistry::instance().register_values(enumName, values);
}


// required here due to import quirks. yayy
Ref<Nyanners::Instances::Object>
Nyanners::Scripting::Reflection::get_object_from_lua(
	lua_State *context, const int index
) {
	const auto *reflectionInstance =
			ReflectionService::get_instance_from_context(context, index);

	return reflectionInstance->pointer;
}

void Nyanners::Scripting::Reflection::push_object_to_lua(
	lua_State *context, const Ref<Nyanners::Instances::Object> &object
) {
	if (object == nullptr) {
		lua_pushnil(context);
		return;
	}
	ReflectionService::reflect_class(context, object);
}

ReflectionInstance *
ReflectionService::get_instance_from_context(lua_State *context, const int id) {
	auto *instance = static_cast<ReflectionInstance *>(
		lua_touserdatatagged(context, id, LUA_SCRIPT_INSTANCE_TAG)
	);

	if (instance == nullptr || instance->pointer == nullptr) {
		luaL_error(context, "BUG: Attempt to pull an Instance that was never teased to Lua");
	}

	return instance;
}

std::vector<ReflectionProperty> ReflectionService::get_properties(
	const Ref<Instances::Object> &instance
) {
	const auto registry = ReflectionDescriptorRegistry::instance();
	const auto defaultDescriptor = registry->descriptors.find("Object");

	if (!does_descriptor_exist(instance->baseName)) {
		return defaultDescriptor->second.properties;
	}

	const auto descriptor = registry->descriptors.find(instance->baseName);
	std::vector<ReflectionProperty> newProperties;
	std::vector<ReflectionDescriptor *> parents;

	construct_family_tree(descriptor->second, parents);
	std::ranges::reverse(parents.begin(), parents.end());

	for (const auto &parent: parents) {
		for (const auto &property: parent->properties) {
			newProperties.push_back(property);
		}
	}
	for (const auto &property: descriptor->second.properties) {
		newProperties.push_back(property);
	}

	return newProperties;
}

void ReflectionService::reflect_class(
	lua_State *context, const Ref<Instances::Object> &instance
) {
	if (instance == nullptr) {
		throw std::runtime_error("Attempt to reflect a class that is null");
		return;
	}

	auto registry = ReflectionDescriptorRegistry::instance();
	auto descriptor = registry->descriptors.find(instance->baseName);
	auto instanceDescriptor = registry->descriptors.find("Object");

	if (descriptor == registry->descriptors.end()) {
		Core::Logger::log(
			std::format(
				"Class {} is missing a Reflection descriptor", instance->baseName
			)
		);

		if (instanceDescriptor == registry->descriptors.end()) {
			EngineService::panic("No default descriptor found! Mayday");
		}

		descriptor = instanceDescriptor;
	}
	auto *selfUser =
			lua_newuserdatadtor(context, sizeof(ReflectionInstance), [](void *ptr) {
				auto instance = static_cast<ReflectionInstance *>(ptr);
				Core::Logger::log_debug(
					std::format(
						"Lua is clearing it's reference to instance {} of type {}",
						instance->pointer->name,
						instance->pointer->baseName
					)
				);
				instance->pointer.reset();
				instance->~ReflectionInstance();
			});

	lua_setuserdatatag(context, -1, LUA_SCRIPT_INSTANCE_TAG);

	new(selfUser) ReflectionInstance{
		.pointer = instance,
		.descriptor = &descriptor->second,
	};

	if (
		const int type = luaL_getmetatable(context, "instance"); type != LUA_TTABLE
	) {
		lua_pop(context, 1);
		create_instance_metatable(context);
	}

	lua_setmetatable(context, -2);
}

ReflectionDescriptor &ReflectionService::create_descriptor(
	const std::string &className,
	const std::vector<std::string> &parents,
	const std::vector<ReflectionInstanceFlags> &flags
) {
	const auto registry = ReflectionDescriptorRegistry::instance();
	ReflectionDescriptor descriptor{className};

	for (const auto &flag: flags) {
		descriptor.flags |= static_cast<uint8_t>(flag);
	}

	descriptor.pending_parents = parents;

	const auto [iterator, _] =
			registry->descriptors.emplace(className, descriptor);
	return iterator->second;
}

int ReflectionService::handle_property(
	lua_State *context,
	const std::string &propertyName,
	const ReflectionInstance *instance,
	const ReflectionDescriptor &descriptor
) {
	if (
		const auto &property = descriptor.get_property(propertyName);
		property != std::nullopt
	) {
		ReflectionValue value;
		property->get(instance->pointer.get(), value, context);

		switch (property->type) {
			case (Boolean): {
				lua_pushboolean(context, std::get<bool>(value));
				return 1;
			};
			case (Number): {
				lua_pushnumber(context, std::get<double>(value));
				return 1;
			};
			case (Integer): {
				lua_pushnumber(context, std::get<int>(value));
				return 1;
			};
			case (Enum): {
				lua_pushinteger(context, std::get<int>(value));
				return 1;
			}
			case (String): {
				lua_pushstring(context, std::get<std::string>(value).c_str());
				return 1;
			}
			case (Vector2): {
				push_vector2(context, std::get<glm::vec2>(value));
				return 1;
			}
			case (Vector3): {
				push_vector3(context, std::get<glm::vec3>(value));
				return 1;
			}
			case (Color): {
				push_color3(context, std::get<DataTypes::Color3>(value));
				return 1;
			}
			case (ReflectionPropertyType::Null): {
				lua_pushnil(context);
			}
			case (ReflectionPropertyType::Instance): {
				reflect_class(
					context, std::get<Ref<Instances::Object> >(value)
				);
				return 1;
			}

			default: {
				throw std::invalid_argument("Unknown property type");
			};
		}
	}

	if (
		const auto &method = descriptor.get_method(propertyName.data());
		method != std::nullopt
	) {
		auto *methodData = static_cast<ReflectionMethodCallback *>(
			lua_newuserdatatagged(
				context, sizeof(ReflectionMethodCallback), LUA_PROPERTY_METHOD_TAG
			)
		);
		*methodData = method->call;

		lua_pushcclosure(
			context,
			[](lua_State *context) {
			auto *methodCallback =
			static_cast<ReflectionMethodCallback *>(lua_touserdatatagged(
				context, lua_upvalueindex(1), LUA_PROPERTY_METHOD_TAG
			));
			const auto &instance = get_instance_from_context(context, 1);

			return (*methodCallback)(instance->pointer.get(), context);
			},
			method->name.c_str(),
			1
		);

		return 1;
	}

	if (
		const auto instanceObject =
				std::dynamic_pointer_cast<Instance>(instance->pointer)
	) {
		if (
			const auto child =
					instanceObject->find_first_child<Instance>(propertyName.data())
		) {
			reflect_class(context, child);
			return 1;
		}
	}

	return 0;
}

bool ReflectionService::handle_new_value(
	lua_State *context,
	const std::string &propertyName,
	const ReflectionInstance *instance,
	const ReflectionDescriptor &descriptor
) {
	if (
		const auto &property = descriptor.get_property(propertyName.data());
		property != std::nullopt
	) {
		// read-only
		if (property->flags & 0) {
			luaL_error(context, "Cannot modify a read-only property");
		}

		switch (property->type) {
			case (Boolean): {
				property->set(
					instance->pointer.get(), read_value<bool>(context, -1), context
				);
				return true;
			};
			case (Number): {
				property->set(
					instance->pointer.get(), read_value<double>(context, -1), context
				);
				return true;
			};
			case (Integer): {
				property->set(
					instance->pointer.get(), read_value<int>(context, -1), context
				);
				return true;
			};
			case (Enum): {
				property->set(
					instance->pointer.get(), read_value<int>(context, -1), context
				);
				return true;
			};
			case (String): {
				property->set(
					instance->pointer.get(), read_value<std::string>(context, -1), context
				);
				return true;
			}
			case (Vector2): {
				property->set(
					instance->pointer.get(), read_value<glm::vec2>(context, -1), context
				);
				return true;
			}
			case (Vector3): {
				property->set(
					instance->pointer.get(), read_value<glm::vec3>(context, -1), context
				);
				return true;
			}
			case (Color): {
				property->set(
					instance->pointer.get(),
					read_value<DataTypes::Color3>(context, -1),
					context
				);
				return true;
			}
			case (ReflectionPropertyType::Instance): {
				property->set(
					instance->pointer.get(),
					get_instance_from_context(context, -1)->pointer,
					context
				);
				return true;
			}

			default: {
				throw std::invalid_argument("Unknown property type");
			};
		}
	}

	return false;
}

void ReflectionService::push_struct(
	lua_State *context, const std::map<std::string, ReflectionValue> &map
) {
	lua_newtable(context);
	const int stackTop = lua_gettop(context);

	for (const auto &[key, value]: map) {
		lua_pushlstring(context, key.c_str(), key.size());
		push_value(context, value);
		lua_settable(context, stackTop);
	}
}

void ReflectionService::construct_family_tree(
	const ReflectionDescriptor &start,
	std::vector<ReflectionDescriptor *> &descriptors
) {
	if (start.parents.empty()) {
		return;
	}

	for (const auto &parent: start.parents) {
		// avoid duplicate elements
		if (std::ranges::find(descriptors, parent) != descriptors.end()) {
			// Core::Logger::log_debug("BUG: Element has the same parent twice (inherited? element defines a parent that is the child of the first parent?)");
			continue;
		}

		descriptors.push_back(parent);
		construct_family_tree(*parent, descriptors);
	}
}

int ReflectionService::instance_index(
	lua_State *context, const ReflectionInstance *instance
) {
	const std::string propertyName = luaL_checkstring(context, -1);

	if (instance == nullptr) {
		Core::Logger::log_debug("ReflectionInstance has been set to null");
		lua_pushnil(context);
		return 0;
	}

	if (instance->pointer == nullptr) {
		Core::Logger::log_debug(
			"Lua attempted to use Instance after it went on the path of destruction"
		);
		lua_pushnil(context);
		return 0;
	}

	int result =
			handle_property(context, propertyName, instance, *instance->descriptor);

	if (result != 0) {
		return result;
	}

	luaL_error(
		context,
		std::format(
			"{}::{} is an invalid property and or child",
			instance->pointer->baseName,
			propertyName
		)
		.c_str()
	);
}

int ReflectionService::instance_new_index(
	lua_State *context, const ReflectionInstance *instance
) {
	const std::string propertyName = luaL_checkstring(context, -2);

	if (instance == nullptr) {
		Core::Logger::log_debug("ReflectionInstance has been set to null");
		lua_pushnil(context);
		return 0;
	}

	if (instance->pointer == nullptr) {
		Core::Logger::log_debug(
			"Lua attempted to use Instance after it went on the path of destruction"
		);
		lua_pushnil(context);
		return 0;
	}

	if (
		handle_new_value(context, propertyName, instance, *instance->descriptor)
	) {
		return 0;
	}

	luaL_error(
		context,
		std::format(
			"{}::{} is an invalid property", instance->pointer->baseName, propertyName
		)
		.c_str()
	);
}

void ReflectionService::register_pending_parents() {
	const auto registry = ReflectionDescriptorRegistry::instance();

	for (auto &descriptor: registry->descriptors | std::views::values) {
		auto &pendingParents = descriptor.pending_parents;
		std::vector<ReflectionDescriptor *> parents;

		if (pendingParents.empty()) {
			continue;
		}

		for (const auto &parent: pendingParents) {
			if (!does_descriptor_exist(parent)) {
				Core::Logger::log_error(
					"!Fake class warning! Uh-oh! This class created a descriptor with a non-existent parent!"
				);
				continue;
			}

			const auto parentDescriptor = registry->descriptors.find(parent);

			if (parentDescriptor != registry->descriptors.end()) {
				descriptor.parents.push_back(&parentDescriptor->second);
			}
		}

		std::vector<ReflectionDescriptor *> tree;
		construct_family_tree(descriptor, parents);
	}
}

NativeData* ReflectionService::generate_lua_reflection_table() {
	auto* root = new NativeData();

	for (const auto &descriptor:
	     ReflectionDescriptorRegistry::instance()->descriptors |
	     std::views::values) {

		auto* descriptorData = new NativeData();
		descriptorData->set_value("Name", descriptor.name);
		descriptorData->set_value("Flags", descriptor.flags);

		auto* propertiesData = new NativeData();
		auto* parentsData = new NativeData();
		auto* methodsData = new NativeData();

		for (const auto &property: descriptor.properties) {
			auto propertyData = new NativeData();

			propertyData->set_value("Name", property.name);
			propertyData->set_value("Type", reflection_property_type_to_string(property.type));

			propertiesData->add_child(propertyData);
		}

		for (const auto &parent: descriptor.parents) {
			parentsData->add_child(parent->name);
		}

		for (const auto &method: descriptor.methods) {
			auto* methodData = new NativeData();
			methodData->set_value("Name", method.name);
			methodData->set_value("Type", reflection_property_type_to_string(method.returnType));
			methodData->set_value("Signature", std::format("{}:{}() -> {}", descriptor.name, method.name, reflection_property_type_to_string(method.returnType)));

			auto* parametersData = new NativeData();

			for (const auto &parameter: method.parameters) {
				auto* parameterData = new NativeData();
				parameterData->set_value("Name", parameter.name);
				parameterData->set_value("Type", reflection_property_type_to_string(parameter.type));

				parametersData->add_child(parameterData);
			}

			methodData->set_value("Parameters", parametersData);
			methodsData->add_child(methodData);
		}

		descriptorData->set_value("Properties", propertiesData);
		descriptorData->set_value("Parents", parentsData);
		descriptorData->set_value("Methods", methodsData);

		root->add_child(descriptorData);
	}

	return root;
}

void ReflectionService::push_value(
	lua_State *context, const ReflectionValue &value
) {
	if (std::holds_alternative<std::string>(value)) {
		const std::string cStr = std::get<std::string>(value);
		lua_pushlstring(context, cStr.c_str(), cStr.size());
		return;
	}
	if (std::holds_alternative<int>(value)) {
		lua_pushnumber(context, std::get<int>(value));
		return;
	}
	if (std::holds_alternative<double>(value)) {
		lua_pushnumber(context, std::get<double>(value));
	}
}

void ReflectionService::create_instance_metatable(lua_State *context) {
	if (luaL_newmetatable(context, "instance")) {
		constexpr luaL_Reg sRegs[] = {
			{
				"__index",
				[](lua_State *context) {
					auto *instance = get_instance_from_context(context, 1);
					if (instance == nullptr) {
						throw std::runtime_error(
							"Instance userdata is null or invalid userdata passed"
						);
					}

					return instance_index(context, instance);
				}
			},
			{
				"__newindex",
				[](lua_State *context) {
					auto *instance = get_instance_from_context(context, 1);
					if (instance == nullptr) {
						throw std::runtime_error(
							"Instance userdata is null or invalid userdata passed"
						);
					}

					return instance_new_index(context, instance);
				}
			},
			{
				"__tostring",
				[](lua_State *context) {
					lua_pushstring(context, "Instance");
					return 1;
				},
			},
			{nullptr, nullptr}
		};

		luaL_register(context, nullptr, sRegs);
		lua_setreadonly(context, -1, true);
	} else {
		Core::Logger::log_error("Could not create Instance metatable");
	}
}

void ReflectionService::register_reflections() {
	create_descriptor("Object", {}, {ReflectionInstanceFlags::NotCreatable})
			.add_property_chained<Object, bool, &Object::get_active, &Object::set_active>("Active", Boolean)
			.add_property_chained<Object, std::string, &Object::get_name, &Object::set_name>("Name", String)
			.add_property_chained<Object, std::string, &Object::get_basename>("ClassName", String);
	create_descriptor("Instance", {"Object"}, {ReflectionInstanceFlags::NotCreatable})
			.add_property_chained<Instance, Ref<Object>, &Instance::get_parent_object, &Instance::set_parent_object>("Parent", ReflectionPropertyType::Instance)
			.add_method<&Instance::clone>("clone", ReflectionPropertyType::Instance, {})
			.add_method<Instance, &Instance::destroy_lua>("destroy", Null);
	create_descriptor("DataModel", {"Instance"}, {ReflectionInstanceFlags::Service})
			.add_method<Instances::DataModel, &Instances::DataModel::get_service_lua>("get_service", ReflectionPropertyType::Instance)
			.add_method_anon("shutdown",[](Object*, lua_State*) -> int {
				Application::instance()->shutdown();
				return 0;
			},Null);
	create_descriptor("Transformable", {"Instance"}, {ReflectionInstanceFlags::NotCreatable})
			.add_property_chained<Instances::Transformable, glm::vec3, &Instances::Transformable::get_position, &Instances::Drawable::set_position>("Position", Vector3)
			.add_property_chained<Instances::Transformable, glm::vec3, &Instances::Transformable::get_rotation, &Instances::Drawable::set_rotation>("Rotation", Vector3)
			.add_property_chained<Instances::Transformable, glm::vec3, &Instances::Transformable::get_scale, &Instances::Drawable::set_scale>("Scale", Vector3);

	create_descriptor("MeshPart", {"Transformable"})
			.add_method<&Instances::MeshPart::load_from_obj_file>("load_from_file", Null, {{"ModelPath", String}})
			.add_constructor<Instances::MeshPart>();
	create_descriptor("RenderingService",{"Instance"},{ReflectionInstanceFlags::NotCreatable, ReflectionInstanceFlags::Service})
			.add_property_chained<RenderingService, double, &RenderingService::get_fps>("FPS", Number)
			.add_property_chained<RenderingService, glm::vec2, &RenderingService::get_window_size>("ViewportSize", Vector2)
			.add_method<&RenderingService::set_window_title>("set_window_title", Null, {});
	create_descriptor("RunService", {"Instance"}, {ReflectionInstanceFlags::NotCreatable, ReflectionInstanceFlags::Service})
			.add_property<RunService, Ref<Instances::SignalBase>, &RunService::get_on_tick>("Tick", ReflectionPropertyType::Instance);

	create_descriptor("Signal", {"Instance"})
			.add_method<&Instances::SignalBase::connectLua>("Connect", Unknown, {{"Arguments", Anything}});
	Instances::link_basic_containers();
	ReflectionDescriptorRegistry::instance()->flush_registrators();
	// parents have to be done separately, to ensure all descriptors are registered
	// as otherwise this creates cases where e.g, Button has Drawable as a parent, but because Drawable is after Button, Button gets a "invalid parent" error.
	register_pending_parents();
}
