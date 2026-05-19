#include "ReflectionService.h"
#include "Application.h"
#include "EngineService.h"
#include "RenderingService.h"
#include "lualib.h"
#include "core/Logger.h"
#include "instances/Script.h"
#include "instances/basic/Signal.h"
#include "instances/drawable/TextLabel.h"
#include "instances/services/RunService.h"
#include "scripting/data/UserdataTags.h"
#include "scripting/reflections/DataTypes.h"
#include "scripting/reflections/ReflectionPropertyReaders.h"
#include "scripting/reflections/ReflectionTypes.h"
#include <algorithm>

using namespace Nyanners::Services;
using namespace Nyanners::Scripting::Reflection;

ReflectionInstance *
ReflectionService::get_instance_from_context(lua_State *context, const int id) {
	auto *instance = static_cast<ReflectionInstance *>(
	  lua_touserdatatagged(context, id, LUA_SCRIPT_INSTANCE_TAG)
	);

	return instance;
}

std::vector<ReflectionProperty>
ReflectionService::get_properties(const std::shared_ptr<Instance> &instance) {
	const auto registry = ReflectionDescriptorRegistry::instance();
	const auto defaultDescriptor = registry->descriptors.find("Instance");

	if (!does_descriptor_exist(instance->baseName)) {
		return defaultDescriptor->second.properties;
	}

	const auto descriptor = registry->descriptors.find(instance->baseName);
	std::vector<ReflectionProperty> newProperties;
	std::vector<ReflectionDescriptor *> parents;

	construct_family_tree(descriptor->second, parents);
	std::ranges::reverse(parents.begin(), parents.end());

	for (const auto &parent : descriptor->second.parents) {
		for (const auto &property : parent->properties) {
			newProperties.push_back(property);
		}
	}
	for (const auto &property : descriptor->second.properties) {
		newProperties.push_back(property);
	}

	return newProperties;
}

void ReflectionService::reflect_class(
  lua_State *context, const std::shared_ptr<Instance> &instance
) {
	auto registry = ReflectionDescriptorRegistry::instance();
	auto descriptor = registry->descriptors.find(instance->baseName);
	auto instanceDescriptor = registry->descriptors.find("Instance");
	// ReflectionDescriptor* descriptor = nullptr;

	if (descriptor == registry->descriptors.end()) {
		Core::Logger::log(std::format("Class {} is missing a Reflection descriptor", instance->baseName));

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

	new (selfUser) ReflectionInstance{
	  .pointer = instance,
	  .descriptor = &descriptor->second,
	};

	if (luaL_newmetatable(context, "instance")) {
		constexpr luaL_Reg sRegs[] = {
		  {"__index",
		   [](lua_State *context) {
			   auto *instance = get_instance_from_context(context, 1);
			   if (instance == nullptr) {
				   throw std::runtime_error(
				     "Instance userdata is null or invalid userdata passed"
				   );
			   }

			   return instance_index(context, instance);
		   }},
		  {"__newindex",
		   [](lua_State *context) {
			   auto *instance = get_instance_from_context(context, 1);
			   if (instance == nullptr) {
				   throw std::runtime_error(
				     "Instance userdata is null or invalid userdata passed"
				   );
			   }

			   return instance_new_index(context, instance);
		   }},
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
	}

	lua_setreadonly(context, -1, true);
	lua_setmetatable(context, -2);
}

ReflectionDescriptor &ReflectionService::create_descriptor(
  const std::string &className, const std::vector<std::string> &parents
) {
	const auto registry = ReflectionDescriptorRegistry::instance();
	ReflectionDescriptor descriptor{className};

	for (const auto &parent : parents) {
		if (!does_descriptor_exist(parent)) {
			Core::Logger::log_error(
			"!Fake class warning! Uh-oh! This class created a descriptor either before the parent or with a non-existent parent!"
			);
			continue;
		}
		auto parentDescriptor = registry->descriptors.find(parent);

		if (parentDescriptor != registry->descriptors.end()) {
			descriptor.parents.push_back(&parentDescriptor->second);
		}
	}

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
	if (const auto& property = descriptor.get_property(propertyName.data()); property != std::nullopt) {
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
			case (ReflectionPropertyType::Instance): {
				reflect_class(context, std::get<std::shared_ptr<Instances::Instance>>(value));
				return 1;
			}

			default: {
				throw std::invalid_argument("Unknown property type");
			};
		}

		return 0;
	}

	if (const auto& method = descriptor.get_method(propertyName.data()); method != std::nullopt) {
		auto **methodData =
			static_cast<const ReflectionMethodCallback **>(lua_newuserdatatagged(
			context, sizeof(ReflectionMethod *), LUA_PROPERTY_METHOD_TAG
			));
		*methodData = &method->call;

		lua_pushcclosure(
		context,
		[](lua_State *context) {
			auto **methodCallback =
				static_cast<ReflectionMethodCallback **>(lua_touserdatatagged(
				context, lua_upvalueindex(1), LUA_PROPERTY_METHOD_TAG
				));
			auto instance = get_instance_from_context(context, 1);

			return (**methodCallback)(instance->pointer.get(), context);
		}, method->name.c_str(), 1);
		return 1;
	}

	if (auto child =
	      instance->pointer->find_first_child<Instance>(propertyName.data())) {
		reflect_class(context, child);
		return 1;
	}

	return 0;
}

bool ReflectionService::handle_new_value(
  lua_State *context,
  const std::string &propertyName,
  const ReflectionInstance *instance,
  const ReflectionDescriptor &descriptor
) {
	if (const auto& property = descriptor.get_property(propertyName.data()); property != std::nullopt) {
		// read-only
		if (property->flags & 0) {
			luaL_error(context, "Cannot modify a read-only property");
		}

		switch (property->type) {
			case (Boolean): {
				property->set(instance->pointer.get(), read_value<bool>(context, -1), context);
				return true;
			};
			case (Number): {
				property->set(instance->pointer.get(), read_value<double>(context, -1), context);
				return true;
			};
			case (String): {
				property->set(instance->pointer.get(), read_value<std::string>(context, -1), context);
				return true;
			}
			case (Vector2): {
				property->set(instance->pointer.get(), read_value<glm::vec2>(context, -1), context);
				return true;
			}
			case (Vector3): {
				property->set(instance->pointer.get(), read_value<glm::vec3>(context, -1), context);
				return true;
			}
			case (Color): {
				property->set(instance->pointer.get(), read_value<DataTypes::Color3>(context, -1), context);
				return true;
			}
			case (ReflectionPropertyType::Instance): {
				property->set(instance->pointer.get(), get_instance_from_context(context, -1)->pointer, context);
				return true;
			}

			default: {
				throw std::invalid_argument("Unknown property type");
			};
		}
	}

	return false;
}

void ReflectionService::construct_family_tree(
  const ReflectionDescriptor& start, std::vector<ReflectionDescriptor*> &descriptors
) {
	for (const auto& parent : start.parents) {
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

	int result = handle_property(context, propertyName, instance, *instance->descriptor);

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

	return 0;
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

	if (handle_new_value(context, propertyName, instance, *instance->descriptor)) {
		return 0;
	} else {
		luaL_error(
		  context,
		  std::format("{}::{} is an invalid property", instance->pointer->baseName, propertyName).c_str()
		);
	}
}

void ReflectionService::register_reflections() {
	create_descriptor("Instance", {})
		.add_property_chained<Instance, std::string, &Instance::get_name, &Instance::set_name>("Name", String)
		.add_property_chained<Instance, bool, &Instance::get_active, &Instance::set_active>("Active", Boolean);

	create_descriptor("DataModel", {"Instance"})
	.add_method<Instances::DataModel, &Instances::DataModel::get_service_lua>("get_service", Boolean);

	create_descriptor("Transformable", {"Instance"})
	.add_property_chained<Instances::Transformable, glm::vec3, &Instances::Transformable::get_position, &Instances::Drawable::set_position>("Position", Vector3)
	.add_property_chained<Instances::Transformable, glm::vec3, &Instances::Transformable::get_rotation, &Instances::Drawable::set_rotation>("Rotation", Vector3)
	.add_property_chained<Instances::Transformable, glm::vec3, &Instances::Transformable::get_scale, &Instances::Drawable::set_scale>("Scale", Vector3);

	create_descriptor("MeshPart", {"Transformable"});

	create_descriptor("RenderingService", {"Instance"})
	.add_property<RenderingService, double, &RenderingService::get_fps>("FPS", Number);

	Instances::link_basic_containers();
	ReflectionDescriptorRegistry::instance()->flush_registrators();
}