#pragma once
#include <functional>

#include "instances/Instance.h"
#include "lua.h"

#include <map>
#include <variant>

#define LUA_SCRIPT_INSTANCE_TAG 0x02

enum ReflectionPropertyType
{
  Unknown,
  String,
  Number,
  Instance,
  // void*
  Method,
  UserData,
};

using ReflectionMethod = std::function<int(lua_State*)>;
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

struct ReflectionClass {
  const std::string className = "Instance";
  const std::string base = "Instance";
  const bool isService = false;

  const ReflectionConstructor constructor;
  const std::vector<ReflectionProperty> properties;
};

struct ReflectionInstance
{
  std::shared_ptr<Nyanners::Instances::Instance> pointer;
  const ReflectionClass* descriptor;
};

namespace Nyanners::Services {
  class ReflectionService : public Instances::Instance {
  public:
    static std::map<std::string, ReflectionClass> classes;
    ReflectionService() : Instance("ReflectionService") {};

    static void reflect_class(lua_State* context, const std::shared_ptr<Instance>& instance);
    static ReflectionClass create_reflection(const ReflectionClass& descriptor);
    static ReflectionInstance* get_instance_from_context(lua_State* context, const int id);
    static void register_reflections();
  private:
    static int instance_index(lua_State* context, ReflectionInstance* instance);
    static int instance_new_index(lua_State* context, const ReflectionInstance* instance);
  };

}