#pragma once
#include "lua.h"
#include "lualib.h"
#include "instances/Instance.h"
#include "instances/datatypes/Vector.h"
#include "scripting/reflections/ReflectionTypes.h"
#include "instances/container/BasicContainers.h"
#include <algorithm>
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
  Method,
  UserData,
};

using ReflectionVariant = std::variant<
	std::monostate, // void
	bool,
	int,
	double,
	float,
	std::string,
	std::shared_ptr<Nyanners::Instances::Instance>,
	Nyanners::DataTypes::Vector3,
	Nyanners::DataTypes::Vector2
>;

using ReflectionGetter = std::function<int(const Nyanners::Instances::Instance*, lua_State*)>;
using ReflectionSetter = std::function<void(Nyanners::Instances::Instance*, lua_State*)>;
using ReflectionConstructor = std::function<std::shared_ptr<Nyanners::Instances::Instance>()>;
using InstanceFlags = std::array<Nyanners::Scripting::Reflection::ReflectionInstanceFlags, 3>;
using PropertyFlags = std::array<Nyanners::Scripting::Reflection::ReflectionPropertyFlags, 3>;

struct ReflectionProperty {
  const std::string name;
  const ReflectionPropertyType type = Unknown;
	const PropertyFlags flags;

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
	const InstanceFlags flags;

  const ReflectionConstructor constructor;
  std::vector<ReflectionProperty> properties;
	std::vector<ReflectionMethod> methods;
};

struct ReflectionInstance
{
  std::shared_ptr<Nyanners::Instances::Instance> pointer;
  ReflectionClass* descriptor;
};

namespace Nyanners::Scripting::Reflection {
	template <typename T>
	const T& get_value_from_variant(const ReflectionVariant& v);

	template <>
	inline const std::string& get_value_from_variant<std::string>(const ReflectionVariant& v) {
		return std::get<std::string>(v);
	}

	template <>
	inline const bool& get_value_from_variant<bool>(const ReflectionVariant& v) {
		return std::get<bool>(v);
	}

	template <>
	inline const int& get_value_from_variant<int>(const ReflectionVariant& v) {
		return std::get<int>(v);
	}

	template <>
	inline const float& get_value_from_variant<float>(const ReflectionVariant& v) {
		return std::get<float>(v);
	}

	template <typename T>
	std::shared_ptr<Nyanners::Instances::Instance> create_instance() {
		static_assert(std::is_base_of_v<Nyanners::Instances::Instance, T>);

		return std::make_shared<T>();
	}
}

namespace Nyanners::Services {
  class ReflectionService : public Instances::Instance {
  public:
    static std::map<std::string, ReflectionClass> classes;
    ReflectionService() : Instance("ReflectionService") {
    	Instances::link_basic_containers();
    };

    static void reflect_class(lua_State* context, const std::shared_ptr<Instance>& instance);
    static ReflectionClass& create_reflection(const ReflectionClass &descriptor);
		;
  	static void add_property(ReflectionClass& descriptor, const ReflectionProperty& property);
  	template <typename internalType, typename get, typename set>
  	static void add_property_new(
  		ReflectionClass& descriptor,
  		const ReflectionPropertyType type,
  		std::string name,

  		get&& getter,
  		set&& setter
  		) {
  		ReflectionProperty property = {
  			.name = std::move(name),
				.type = type,
			};

  		property.get = [getter = std::forward<get>(getter)](const Instance* i) {
  			return ReflectionValue(getter(i));
  		};

  		property.set = [setter = std::forward<set>(setter)](const Instance* i, const ReflectionVariant& variant) {
  			setter(i, Scripting::Reflection::get_value_from_variant<internalType>(variant));
  		};

  		descriptor.properties.push_back(std::move(property));
  	};
  	static void add_method(ReflectionClass& descriptor, const ReflectionMethod& method);

    static ReflectionInstance* get_instance_from_context(lua_State* context, const int id);
  	static ReflectionClass* get_descriptor(std::string className);
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