#include "ReflectionService.h"
#include "Application.h"
#include "EngineService.h"
#include "IOService.h"
#include "RenderingService.h"
#include "RunService.h"
#include "UIService.h"
#include "lualib.h"
#include "core/Logger.h"
#include "instances/DataModel.h"
#include "instances/basic/Signal.h"
#include "instances/drawable/TextLabel.h"
#include "scripting/data/UserdataTags.h"
#include "scripting/reflections/DataTypes.h"
#include "scripting/reflections/ReflectionTypes.h"
#include <algorithm>
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

ReflectionClass *
ReflectionService::get_descriptor(const std::string className) {
	const auto descriptor = classes.find(className);

	if (descriptor != classes.end()) {
		return &descriptor->second;
	} else {
		return nullptr;
	}
}

std::vector<ReflectionProperty>
ReflectionService::get_properties(const std::shared_ptr<Instance> &instance) {
	const auto descriptor = classes.find(instance->baseName);
	const auto defaultDescriptor = classes.find("Instance");

	if (descriptor == classes.end()) {
		return defaultDescriptor->second.properties;
	} else {
		std::vector<ReflectionProperty> newProperties;
		std::vector<ReflectionClass*> tree;
		auto tempDescriptor = get_descriptor(descriptor->second.base);

		while (tempDescriptor != nullptr) {
			tree.push_back(tempDescriptor);
			tempDescriptor = get_descriptor(tempDescriptor->base);
		}

		std::reverse(tree.begin(), tree.end());

		for (const auto& parent : tree) {
			for (const auto &property : parent->properties) {
				newProperties.push_back(property);
			}
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

ReflectionClass &
ReflectionService::create_reflection(const ReflectionClass &descriptor) {
	auto existingClass = classes.find(descriptor.className);

	if (existingClass != classes.end()) {
		return existingClass->second;
	}

	const auto reflectionClass = std::make_pair(descriptor.className, descriptor);

	const auto [iterator, _] = classes.emplace(reflectionClass);

	return iterator->second;
}
void ReflectionService::add_property(
  ReflectionClass &descriptor, const ReflectionProperty &property
) {
	descriptor.properties.push_back(property);
}

void ReflectionService::add_method(
  ReflectionClass &descriptor, const ReflectionMethod &method
) {
	descriptor.methods.push_back(method);
}

int ReflectionService::handle_property(
  lua_State *context,
  std::string_view propertyName,
  const ReflectionInstance *instance,
  const ReflectionClass &descriptor
) {
	for (const auto &property : descriptor.properties) {
		if (property.name == propertyName) {
			return property.get(instance->pointer.get(), context);
		}
	}

	for (auto &method : descriptor.methods) {
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

	if (auto child =
	      instance->pointer->find_first_child<Instance>(propertyName.data())) {
		reflect_class(context, child);
		return 1;
	}

	return 0;
}

bool ReflectionService::handle_new_value(
  lua_State *context,
  std::string_view propertyName,
  const ReflectionInstance *instance,
  const ReflectionClass &descriptor
) {
	for (const auto &property : descriptor.properties) {
		if (property.name == propertyName) {
			property.set(instance->pointer.get(), context);
			return true;
		}
	}

	for (const auto &property : descriptor.properties) {
		if (property.name == propertyName) {
			if (std::ranges::contains(property.flags, Scripting::Reflection::ReflectionPropertyFlags::ReadOnly)) {
				luaL_error(context, "Cannot modify a read-only property");
			}

			property.set(instance->pointer.get(), context);
			break;
		}
	}

	return false;
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

	ReflectionClass *descriptor = instance->descriptor;

	while (descriptor != nullptr) {
		int result = handle_property(context, propertyName, instance, *descriptor);

		if (result != 0) {
			return result;
		}

		if (!descriptor->base.empty()) {
			descriptor = get_descriptor(descriptor->base);
		}
	};

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
		throw std::runtime_error("Instance userdata is null");
	}

	if (instance->pointer == nullptr) {
		throw std::runtime_error("Instance pointer is null");
	}

	ReflectionClass *descriptor = instance->descriptor;

	while (descriptor != nullptr) {
		if (handle_new_value(context, propertyName, instance, *descriptor)) {
			return 0;
		};

		if (!descriptor->base.empty()) {
			descriptor = get_descriptor(descriptor->base);
		}
	};

	luaL_error(
	  context,
	  std::format(
	    "{}::{} is an invalid property", instance->pointer->baseName, propertyName
	  )
	    .c_str()
	);
}

void ReflectionService::register_reflections() {
	create_reflection(
		{.className = "Instance",
		 .base = "<<root>>",
			.flags = {Scripting::Reflection::Service},
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
			.flags = {Scripting::Reflection::Service},
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
			.flags = {Scripting::Reflection::Service},
		 .constructor = []() { return std::make_shared<UIService>(); },
		 .properties = {}}
	);

	create_reflection(
		{.className = "RunService",
		 .base = "Instance",
			.flags = {Scripting::Reflection::Service},
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
			.flags = {Scripting::Reflection::Service},
		 .constructor =
			 []() {
				 throw std::runtime_error(
					 "Cannot create an instance of RenderingService"
				 );
				 return nullptr;
			 },
		 .properties = {
			 {.name = "fps",
				.type = ReflectionPropertyType::Number,
				.flags = {Scripting::Reflection::ReflectionPropertyFlags::ReadOnly},
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
			.flags = {Scripting::Reflection::Service},
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
			.flags = {Scripting::Reflection::Service},
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
		{
			.className = "TextLabel",
			.base = "Drawable",
			.flags = {},
		 .properties = {
			 {.name = "Text",
				.type = ReflectionPropertyType::String,
				.get =
					[](const Instance *instance, lua_State *context) {
						const auto *label =
						dynamic_cast<const Instances::TextLabel *>(instance);

						lua_pushstring(context, label->text.c_str());

						return 1;
					},
				.set =
					[](Instance *instance, lua_State *context) {
						auto *label = dynamic_cast<Instances::TextLabel *>(instance);
						const std::string text = luaL_checkstring(context, -1);

						label->set_text(text);
					}},
					 {.name = "MaxVisibleGlyphs",
		 .type = ReflectionPropertyType::Number,
		 .get =
		 [](const Instance *instance, lua_State *context) {
			 const auto *label =
			 dynamic_cast<const Instances::TextLabel *>(instance);

			 lua_pushnumber(context, label->maxVisibleGlyph);

			 return 1;
		 },
		 .set =
		 [](Instance *instance, lua_State *context) {
			 auto *label = dynamic_cast<Instances::TextLabel *>(instance);
			 const int newGlyphs = luaL_checknumber(context, -1);

			 label->maxVisibleGlyph = newGlyphs;
		 }}
		 },
		}
	);

	create_reflection(
		{
			.className = "Signal",
			.flags = {},
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
		{
			.className = "World",
			.flags = {Scripting::Reflection::ReflectionInstanceFlags::Service},
			.properties = {},
			.methods = {}
		}
	);

	auto drawableReflection = create_reflection(
		{.className = "Drawable",
		 .base = "Instance",
			.flags = {},
		 .properties =
			 {{.name = "Color",
				 .type = ReflectionPropertyType::UserData,
				 .get =
					 [](const Instance *instance, lua_State *context) {
						 auto *drawable =
							 dynamic_cast<const Instances::Drawable *>(instance);

						 Scripting::Reflection::push_color3(
		           context, *drawable->material->color
		         );
						 return 1;
					 },
				 .set =
					 [](Instance *instance, lua_State *context) {
						 // Core::Logger::log(
						 //   std::format("type at idx 1 {}", luaL_typename(context, 1))
						 // );
						 // Core::Logger::log(
						 //   std::format("type at idx 2 {}", luaL_typename(context, 2))
						 // );
						 //
						 // Core::Logger::log(
						 //   std::format("type at idx -1 {}", luaL_typename(context, -1))
						 // );
						 // Core::Logger::log(
						 //   std::format("type at idx -2 {}", luaL_typename(context, -2))
						 // );
						 auto *drawable = dynamic_cast<Instances::Drawable *>(instance);
						 auto *color3 =
							 get_userdata_from_context<DataTypes::Color3>(context, -1, 0x05);

						 if (color3 == nullptr) {
							 throw std::runtime_error("Color3 is nullptr");
						 }

						 drawable->material->set_color(color3);

						 return 1;
					 }},
				{.name = "Position",
				 .type = ReflectionPropertyType::UserData,
				 .get =
					 [](const Instance *instance, lua_State *context) {
						 auto *drawable =
							 dynamic_cast<const Instances::Drawable *>(instance);

						 Scripting::Reflection::push_vector3(context, *drawable->position);
						 return 1;
					 },
				 .set =
					 [](Instance *instance, lua_State *context) {
						 auto *drawable = dynamic_cast<Instances::Drawable *>(instance);
						 auto *position =
							 get_userdata_from_context<glm::vec3>(context, -1, 0x06);

						 if (position == nullptr) {
							 throw std::runtime_error("Vector3 is nullptr");
						 }

						 drawable->set_position(*position);

						 return 1;
					 }}},
		 .methods = {}}
	);

	create_reflection({
		.className = "MeshPart",
		.base = "Drawable",
		.flags = {},
		.properties = {},
		.methods = {},
	});

	// auto& frameCounter = create_reflection({
	// 	.className = "FrameCounter",
	// 	.base = "Drawable",
	// 	.isService = false,
	// 	.properties = {},
	// 	.methods = {},
	// });
	//
	// add_property(frameCounter, {
	// 	.name = "<invalid>",
	// 	.readOnly = true,
	// 	.type = String,
	// 	.get =
	// 		[](const Instance *instance, lua_State *context) {
	// 			lua_pushstring(context, "hi");
	// 			return 1;
	// 		},
	// });


	// auto* instance = create_reflection("FrameCounter",
	// 	["Drawable", "TextLabel"],
	// 	[Scripting::Reflection::ReflectionInstanceFlags::Creatable]
	// 	);
	//
	// instance->set_property(
	// 	{
	// 		.name = "Pause",
	// 		.flags = [Scripting::Reflection::ReflectionPropertyFlags::WriteOnly],
	// 		.get = [](lua_State* context, Instance* instance) {
	// 			// automatically turned into a luaL_pushstring?? i guess
	// 			// reflection needs some way to be usable by c++ too, in the case of ExplorerPanel
	// 			return "hi";
	// 		},
	// 		.set = [](lua_State* context, Instance* instance) {
	// 			this->set_value()
	// 		}
	// });
	//
	// register_reflection(instance);
}