#pragma once
#include "lua.h"
#include "instances/Instance.h"
#include "scripting/reflections/ReflectionDescriptor.h"
#include "scripting/reflections/ReflectionDescriptorRegistry.h"
#include "scripting/reflections/ReflectionTypes.h"
#include <functional>
#include <map>
#include <unordered_set>

using namespace Nyanners::Scripting::Reflection;
using namespace Nyanners::Instances;

using ReflectionGetter = std::function<int(const Object*, lua_State *)>;
using ReflectionSetter = std::function<void(Object*, lua_State *)>;
using ReflectionConstructor = std::function<Ref<Nyanners::Instances::Instance>()>;


struct ReflectionClass {
	const std::string className = "Instance";
	const std::string base = "Instance";

	const ReflectionConstructor constructor;
	List<ReflectionProperty> properties;
	List<ReflectionMethod> methods;
};

struct ReflectionInstance {
	Ref<Object> pointer;
	ReflectionDescriptor *descriptor;
};

namespace Nyanners::Services {
	class ReflectionService : public Instance {
	public:
		ReflectionService() : Instance("ReflectionService") {}
		static void reflect_class(lua_State *context, const Ref<Object> &instance);
		static ReflectionDescriptor &create_descriptor(const std::string &className, const List<std::string> &parents, const List<ReflectionInstanceFlags> &flags = {});

		static bool does_descriptor_exist(const std::string &className) {
			return ReflectionDescriptorRegistry::instance()->descriptors.contains(className);
		}
		static ReflectionDescriptor get_descriptor(const std::string &className) {
			return ReflectionDescriptorRegistry::instance()->descriptors.at(className);
		}

		static ReflectionInstance *get_instance_from_context(lua_State *context, const int id);
		static List<ReflectionProperty> get_properties(const Ref<Object> &instance);
		static void register_reflections();
		static void register_enum(const std::string &enumName, const std::map<std::string, int> &values);

		template <typename T>
		static T *get_userdata_from_context(lua_State *context, int idx = 1, int userDataTag = 0) {
			return static_cast<T *>(lua_touserdatatagged(context, idx, userDataTag));
		};

		template <typename T>
		static void create_userdata(lua_State *context, T *data, const int userDataTag) {
			auto *selfUser = static_cast<T *>(
			  lua_newuserdatatagged(context, sizeof(T), userDataTag)
			);

			new (selfUser) T(*data);
		}

		static void push_value(lua_State *context, const ReflectionValue &value);
		static void push_struct(lua_State *context, const std::map<std::string, ReflectionValue> &map);
		NativeData* generate_lua_reflection_table();
	private:
		static void construct_family_tree(const ReflectionDescriptor &start, List<ReflectionDescriptor *> &descriptors);
		static int instance_index(lua_State *context, const ReflectionInstance *instance);
		static int handle_property(lua_State *context, const std::string &propertyName,const ReflectionInstance *instance,const ReflectionDescriptor &descriptor);
		static bool handle_new_value(lua_State *context, const std::string &propertyName, const ReflectionInstance *instance, const ReflectionDescriptor &descriptor);
		static int instance_new_index(lua_State *context, const ReflectionInstance *instance);
		static void register_pending_parents();
		static void create_instance_metatable(lua_State *context);
	};
}