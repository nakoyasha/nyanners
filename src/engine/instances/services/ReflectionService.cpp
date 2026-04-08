#include "ReflectionService.h"
#include "Application.h"
#include "EngineService.h"
#include "IOService.h"
#include "RenderingService.h"
#include "RunService.h"
#include "UIService.h"
#include "lualib.h"
#include "core/Logger.h"
#include "data/UserdataTags.h"
#include "instances/DataModel.h"
#include "instances/basic/Signal.h"
#include "instances/drawable/TextLabel.h"
#include <ranges>

using namespace Nyanners::Services;
std::map<std::string, ReflectionClass> ReflectionService::classes;

ReflectionInstance *
ReflectionService::get_instance_from_context(lua_State *context, const int id) {
	auto *instance = static_cast<ReflectionInstance *>(
	  lua_touserdatatagged(context, id, LUA_SCRIPT_INSTANCE_TAG)
	);

	return instance;
}
std::vector<ReflectionProperty>
ReflectionService::get_properties(const std::shared_ptr<Instance> &instance) {
	const auto descriptor = classes.find(instance->baseName);
	const auto defaultDescriptor = classes.find("Instance");

	if (descriptor == classes.end()) {
		return defaultDescriptor->second.properties;
	} else {
		std::vector<ReflectionProperty> newProperties;

		for (const auto &property : defaultDescriptor->second.properties) {
			newProperties.push_back(property);
		}

		for (const auto &property : descriptor->second.properties) {
			newProperties.push_back(property);
		}

		return newProperties;
	}
}

void ReflectionService::reflect_class(
  lua_State *context, const std::shared_ptr<Instance> &instance
) {
	auto descriptor = classes.find(instance->baseName);

	if (descriptor == classes.end()) {
		Core::Logger::log(
		  std::format(
		    "Class {} is missing a Reflection descriptor", instance->baseName
		  )
		);

		// this is mainly here to make dev easier
		const auto defaultDescriptor = classes.find("Instance");

		if (defaultDescriptor != classes.end()) {
			descriptor = defaultDescriptor;
		}
	}

	auto *selfUser = static_cast<ReflectionInstance *>(lua_newuserdatatagged(
	  context, sizeof(ReflectionInstance), LUA_SCRIPT_INSTANCE_TAG
	));
	new (selfUser) ReflectionInstance{
	  .pointer = instance,
	  .descriptor = &descriptor->second,
	};

	if (luaL_newmetatable(context, "instance")) {
		luaL_Reg sRegs[] = {
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

ReflectionClass
ReflectionService::create_reflection(const ReflectionClass &descriptor) {
	auto existingClass = classes.find(descriptor.className);

	if (existingClass != classes.end()) {
		return existingClass->second;
	}

	classes.insert(std::make_pair(descriptor.className, descriptor));

	return descriptor;
}

int ReflectionService::instance_index(
  lua_State *context, const ReflectionInstance *instance
) {
	const std::string propertyName = luaL_checkstring(context, -1);

	if (instance == nullptr) {
		throw std::runtime_error("Instance userdata is null");
	}

	if (instance->pointer == nullptr) {
		throw std::runtime_error("Instance pointer is null");
	}

	const auto defaultDescriptor = classes.find(instance->descriptor->base);

	if (defaultDescriptor != classes.end()) {
		for (const auto &property : defaultDescriptor->second.properties) {
			if (property.name == propertyName) {
				return property.get(instance->pointer.get(), context);
			}
		}

		for (auto &method : instance->descriptor->methods) {
			if (method.name == propertyName) {
				// oh lord, this is evil.
				auto **methodData =
				  static_cast<const ReflectionMethodCallback **>(lua_newuserdatatagged(
				    context, sizeof(ReflectionMethod *), LUA_PROPERTY_METHOD_TAG
				  ));
				*methodData = &method.method;

				lua_pushcclosure(
				  context,
				  [](lua_State *context) {
					  auto **method =
					    static_cast<ReflectionMethodCallback **>(lua_touserdatatagged(
					      context, lua_upvalueindex(1), LUA_PROPERTY_METHOD_TAG
					    ));
					  auto instance = get_instance_from_context(context, 1);
					  ;

					  return (**method)(instance->pointer, context);
				  },
				  method.name.c_str(),
				  1
				);
				return 1;
			}
		}
	}

	for (const auto &property : instance->descriptor->properties) {
		if (property.name == propertyName) {
			return property.get(instance->pointer.get(), context);
		}
	}

	if (auto child =
	      instance->pointer->find_first_child<Instance>(propertyName)) {
		reflect_class(context, child);
		return 1;
	} else {
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
}

int ReflectionService::instance_new_index(
  lua_State *context, const ReflectionInstance *instance
) {
	const std::string propertyName = luaL_checkstring(context, -2);

	if (instance == nullptr) {
		throw std::runtime_error("Instance userdata is null");
	}

	if (instance->pointer == nullptr) {
		throw std::runtime_error("Instance pointer is null");
	}

	const auto defaultDescriptor = classes.find(instance->descriptor->base);

	if (defaultDescriptor != classes.end()) {
		for (const auto &property : defaultDescriptor->second.properties) {
			if (property.name == propertyName) {
				property.set(instance->pointer.get(), context);
				return 0;
			}
		}
	}

	for (const auto &property : instance->descriptor->properties) {
		if (property.name == propertyName) {
			if (property.readOnly == true) {
				luaL_error(context, "Cannot modify a read-only property");
				return 0;
			}

			property.set(instance->pointer.get(), context);
			return 0;
		}

		luaL_error(
		  context,
		  std::format(
		    "{}::{} is an invalid property",
		    instance->pointer->baseName,
		    propertyName
		  )
		    .c_str()
		);
		return 0;
	}

	return 0;
}

void ReflectionService::register_reflections() {
	create_reflection(
	  {.className = "Instance",
	   .base = "<<root>>",
	   .isService = false,
	   .constructor =
	     []() {
		     throw std::runtime_error("Instance is not a creatable object");
		     return nullptr;
	     },
	   .properties =
	     {
	       {.name = "Active",
	        .type = ReflectionPropertyType::Boolean,
	        .get =
	          [](const Instance *instance, lua_State *context) {
		          lua_pushboolean(context, instance->active);
		          return 1;
	          },
	        .set =
	          [](Instance *instance, lua_State *context) {
		          const auto newBool = luaL_checkboolean(context, -1);
		          instance->set_active(newBool);
		          return 0;
	          }},
	       {.name = "Name",
	        .type = ReflectionPropertyType::String,
	        .get =
	          [](const Instance *instance, lua_State *context) {
		          lua_pushstring(context, instance->name.c_str());
		          return 1;
	          },
	        .set =
	          [](Instance *instance, lua_State *context) {
		          std::string newValue = luaL_checkstring(context, -1);
		          instance->name = newValue;
		          return 0;
	          }},
	       {.name = "ClassName",
	        .type = ReflectionPropertyType::String,
	        .get =
	          [](const Instance *instance, lua_State *context) {
		          lua_pushstring(context, instance->baseName.c_str());
		          return 1;
	          },
	        .set =
	          [](Instance *instance, lua_State *context) {
		          luaL_error(context, "Cannot modify a read-only property");
		          return 0;
	          }},
	       {.name = "Parent",
	        .type = ReflectionPropertyType::String,
	        .get =
	          [](const Instance *instance, lua_State *context) {
		          if (instance->parent == nullptr) {
			          lua_pushnil(context);
			          return 1;
		          }

		          reflect_class(context, instance->parent);
		          return 1;
	          },
	        .set =
	          [](Instance *instance, lua_State *context) {
		          auto *newParent = get_instance_from_context(context, -1);

		          if (newParent == nullptr) {
			          if (instance->parent != nullptr) {
				          instance->parent->remove_child(instance->shared_from_this());
			          }
		          } else {
			          newParent->pointer->add_child(instance->shared_from_this());
		          }

		          return 0;
	          }},
	     },
	   .methods = {
	     {.name = "find_first_child",
	      .method =
	        [](const std::shared_ptr<Instance> instance, lua_State *context) {
		        const std::string childName = luaL_checkstring(context, -1);
		        const auto child = instance->find_first_child<Instance>(childName);

		        if (child == nullptr) {
			        lua_pushnil(context);
			        return 1;
		        }

		        reflect_class(context, child);
		        return 1;
	        }}
	   }}
	);
	create_reflection(
	  {.className = "DataModel",
	   .base = "Instance",
	   .isService = true,
	   .constructor =
	     []() {
		     throw std::runtime_error(
		       "You can't make a DataModel, as it's a singleton."
		     );
		     return nullptr;
	     },
	   .methods = {{
	     .name = "get_service",
	     .method =
	       [](const std::shared_ptr<Instance> instance, lua_State *context) {
		       const std::string service = luaL_checkstring(context, -1);
		       const auto foundService =
		         Application::instance()->currentModel->get_service<Instance>(
		           service
		         );

		       if (foundService == nullptr) {
			       luaL_error(context, "Cannot create non-existent service");
			       return 0;
		       }

		       Services::ReflectionService::reflect_class(context, foundService);
		       return 1;
	       },
	   }}}
	);

	create_reflection(
	  {.className = "UIService",
	   .base = "Instance",
	   .isService = true,
	   .constructor = []() { return std::make_shared<UIService>(); },
	   .properties = {}}
	);

	create_reflection(
	  {.className = "RunService",
	   .base = "Instance",
	   .isService = true,
	   .constructor =
	     []() {
		     throw std::runtime_error("Cannot create an instance of RunService");
		     return nullptr;
	     },
	   .properties = {
	     {.name = "PreRender",
	      .type = ReflectionPropertyType::Instance,
	      .get =
	        [](const Instance *instance, lua_State *context) {
		        const auto *runService = dynamic_cast<const RunService *>(instance);
		        reflect_class(context, runService->preRender);

		        return 1;
	        }},
	     {
	       .name = "OnExit",
	       .type = ReflectionPropertyType::Instance,
	       .get = [](const Instance *instance, lua_State *context) {
		       const auto *runService = dynamic_cast<const RunService *>(instance);
		       reflect_class(context, runService->onStop);

		       return 1;
	       },
	     }
	   }}
	);

	create_reflection(
	  {.className = "RenderingService",
	   .base = "Instance",
	   .isService = true,
	   .constructor =
	     []() {
		     throw std::runtime_error(
		       "Cannot create an instance of RenderingService"
		     );
		     return nullptr;
	     },
	   .properties = {
	     {.name = "fps",
	      .readOnly = true,
	      .type = ReflectionPropertyType::Number,
	      .get = [](const Instance *instance, lua_State *context) {
		      const auto render = static_cast<const RenderingService *>(instance);
		      lua_pushnumber(context, render->fps);
		      return 1;
	      }}
	   }}
	);

	create_reflection(
	  {.className = "IOService",
	   .base = "Instance",
	   .isService = true,
	   .constructor = []() { return std::make_shared<IOService>(); },
	   .methods = {
	     {.name = "read_file",
	      .method =
	        [](const std::shared_ptr<Instance> instance, lua_State *context) {
		        const std::string path = luaL_checkstring(context, -1);

		        try {
			        const std::string result = Services::IOService::read_file(path);
			        lua_pushstring(context, result.c_str());
			        return 1;
		        } catch (std::runtime_error &e) {
			        Core::Logger::log(e.what());
			        luaL_error(context, e.what());
		        }

		        return 1;
	        }}
	   }}
	);

	create_reflection(
	  {.className = "EngineService",
	   .isService = true,
	   .constructor = []() { return std::make_shared<EngineService>(); },
	   .properties = {},
	   .methods = {
	     {.name = "panic",
	      .method =
	        [](const std::shared_ptr<Instance> instance, lua_State *context) {
		        const std::string message = luaL_checkstring(context, -1);
		        EngineService::panic(message);
		        return 0;
	        }}
	   }}
	);

	create_reflection(
	  {.className = "TextLabel",
	   .isService = false,
	   .properties = {
	     {.name = "Text",
	      .type = ReflectionPropertyType::String,
	      .get =
	        [](const Instance *instance, lua_State *context) {
		        // const auto *label =
		        //   dynamic_cast<const Instances::TextLabel *>(instance);
		        // lua_pushstring(context, label->getText().c_str());
	        	lua_pushstring(context, "WIP");

		        return 1;
	        },
	      .set =
	        [](Instance *instance, lua_State *context) {
		        // auto *label = dynamic_cast<Instances::TextLabel *>(instance);
		        // const std::string text = luaL_checkstring(context, -1);
		        //
		        // label->setText(text);
	        }}
	   }}
	);

	create_reflection(
	  {.className = "Signal",
	   .isService = false,
	   .methods = {
	     {.name = "Connect",
	      .method =
	        [](const std::shared_ptr<Instance> &instance, lua_State *context) {
		        const auto signal =
		          std::dynamic_pointer_cast<Instances::SignalBase>(instance);

		        if (signal == nullptr) {
			        throw std::runtime_error(
			          "Signal is nullptr upon access from Lua"
			        );
		        }

		        return signal->connectLua(context);
	        }}
	   }}
	);

	create_reflection(
	  {.className = "World", .isService = true, .properties = {}, .methods = {}}
	);
}
