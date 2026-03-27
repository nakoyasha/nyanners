#pragma once
#include "instances/Instance.h"
#include "lua.h"
#include "lualib.h"

#include <map>
#include <variant>

#define LUA_SCRIPT_INSTANCE_TAG 0x02

struct ReflectionProperty {
  const std::string name;
  const bool readOnly = false;
  std::variant<std::string, float, bool, Nyanners::Instances::Instance *> value;
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

    static void reflect_class(lua_State* context, const ReflectionClass & instance) {
      auto** self = static_cast<Instance**>(lua_newuserdatatagged(context, sizeof(Instance*), LUA_SCRIPT_INSTANCE_TAG));
      *self = instance.pointer;

      if (luaL_newmetatable(context, "instance")) {
        luaL_Reg sRegs[] = {{
          "__index",
          [](lua_State *context) {
            auto* instance = ReflectionService::getInstanceFromContext(context, 1);

            const std::string propertyName =
                luaL_checkstring(context, -1);

            if (propertyName == "Name") {
              lua_pushstring(context, instance->name.c_str());
              return 1;
            }

            return 0;
          },
      },
      {
        "__tostring",
        [](lua_State *context) {
          lua_pushstring(context, "Instance");
          return 1;
        },
    },
    {nullptr, nullptr}};

        luaL_register(context, nullptr, sRegs);
      }
      lua_setmetatable(context, -2);
    }

    static ReflectionClass create_reflection(std::shared_ptr<Instance> instance, std::vector<ReflectionProperty> properties) {
      auto existingClass = classes.find(instance->baseName);

      if (existingClass != classes.end()) {
        return existingClass->second;
      }

      auto reflection = ReflectionClass {
        .className = instance->baseName,
        .base = "Instance",
        .isService = instance->baseName.find("Service") != std::string::npos,
        .pointer = instance.get(),
        .properties = std::move(properties)
      };

      classes.insert(std::pair{instance->baseName, reflection});

      return reflection;
    }
  private:
    static Instance* getInstanceFromContext(lua_State* context, const int id)
    {
      Instance* instance = *static_cast<Instance**>(lua_touserdatatagged(context, id, LUA_SCRIPT_INSTANCE_TAG));

      return instance;
    }
  };

}