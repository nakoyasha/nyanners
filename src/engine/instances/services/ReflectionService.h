#pragma once
#include <functional>

#include "instances/Instance.h"
#include "lua.h"

#include <map>
#include <variant>

#define LUA_SCRIPT_INSTANCE_TAG 0x02

enum ReflectionPropertyType
{
  String,
  Number,
  Instance,
  // void*
  Method,
  UserData,
};

using ReflectionMethod = std::function<int(lua_State*)>;

struct ReflectionProperty {
  const std::string name;
  const bool readOnly = false;
  const ReflectionPropertyType type;
  std::variant<std::string, float, bool, Nyanners::Instances::Instance *, ReflectionMethod, void*> value;
};

struct ReflectionClass {
  const std::string className = "Instance";
  const std::string base = "Instance";
  const bool isService = false;
  Nyanners::Instances::Instance * pointer = nullptr;

  const std::vector<ReflectionProperty> properties;
};

namespace Nyanners::Services {
  class ReflectionService : public Instances::Instance {
  public:
    static std::map<std::string, ReflectionClass> classes;
    ReflectionService() : Instance("ReflectionService") {};

    static void reflect_class(lua_State* context, const ReflectionClass & instance);
    static ReflectionClass create_reflection(const std::shared_ptr<Instance>& instance, std::vector<ReflectionProperty> properties);

  private:
    static Instance* get_instance_from_context(lua_State* context, const int id);
    static int instance_index(lua_State* context);
  };

}