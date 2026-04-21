#pragma once
#include "lua.h"
#include "lualib.h"
#include "instances/Instance.h"
#include <functional>
#include <map>
#include <variant>


enum ReflectionPropertyType
{
  Unknown,
  String,
  Number,
  Boolean,
  Instance,
  // void*
  Method,
  UserData,
};

using ReflectionGetter = std::function<int(const Nyanners::Instances::Instance*, lua_State*)>;
using ReflectionSetter = std::function<void(Nyanners::Instances::Instance*, lua_State*)>;
using ReflectionConstructor = std::function<std::shared_ptr<Nyanners::Instances::Instance>()>;

struct ReflectionProperty {
  const std::string name;
  const bool readOnly = false;
  const ReflectionPropertyType type = ReflectionPropertyType::Unknown;

  const ReflectionGetter get;
  const ReflectionSetter set;
};

using ReflectionMethodCallback = std::function<int(std::shared_ptr<Nyanners::Instances::Instance>, lua_State* context)>;

struct ReflectionMethod {
  const std::string name;
  ReflectionMethodCallback method = [](const std::shared_ptr<Nyanners::Instances::Instance>&, lua_State* context){
    luaL_error(context, "Method is unimplemented");
    return 0;
  };
};

struct ReflectionClass {
  const std::string className = "Instance";
  const std::string base = "Instance";
  const bool isService = false;

  const ReflectionConstructor constructor;
  std::vector<ReflectionProperty> properties;
	std::vector<ReflectionMethod> methods;
};

struct ReflectionInstance
{
  std::shared_ptr<Nyanners::Instances::Instance> pointer;
  ReflectionClass* descriptor;
};
;

namespace Nyanners::Services {
  class ReflectionService : public Instances::Instance {
  public:
    static std::map<std::string, ReflectionClass> classes;
    ReflectionService() : Instance("ReflectionService") {};

    static void reflect_class(lua_State* context, const std::shared_ptr<Instance>& instance);
    static ReflectionClass create_reflection(const ReflectionClass& descriptor);
  	static void add_property(ReflectionClass& descriptor, const ReflectionProperty& property);
  	static void add_method(ReflectionClass& descriptor, const ReflectionMethod& method);
    static ReflectionInstance* get_instance_from_context(lua_State* context, const int id);
  	static ReflectionClass* get_descriptor(const std::string className);
  	static std::vector<ReflectionProperty> get_properties(const std::shared_ptr<Instance>& instance);
    static void register_reflections();

  	template <typename T>
  	static T* get_userdata_from_context(lua_State* context, int idx = 1, int userDataTag = 0) {
  		auto *instance = static_cast<T*>(
				lua_touserdatatagged(context, idx, userDataTag)
			);

  		return instance;
  	};

  	template <typename T>
  	static void create_userdata(lua_State* context, T* data, const int userDataTag) {
  		auto *selfUser = static_cast<T*>(lua_newuserdatatagged(
				context, sizeof(T), userDataTag
			));

  		new (selfUser) T(*data);
  	}
  private:
    static int instance_index(lua_State* context, const ReflectionInstance* instance);
  	static int handle_property(lua_State* context, std::string_view propertyName,const ReflectionInstance* instance, const ReflectionClass& descriptor);
  	static bool handle_new_value(
		  lua_State *context,
		  std::string_view propertyName,
		  const ReflectionInstance *instance,
		  const ReflectionClass &descriptor
		);
    static int instance_new_index(lua_State* context, const ReflectionInstance* instance);
  };

}