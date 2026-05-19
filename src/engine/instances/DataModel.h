#pragma once
#include "Instance.h"
#include <memory>
#include <ranges>

#include "services/ReflectionService.h"

namespace Nyanners::Instances {
  class DataModel : public Instance {
  public:
    DataModel() : Instance("DataModel") {};

  	int test_lua_method(lua_State* context) {
  		lua_pushboolean(context, true);
  		return 1;
  	}

    template <typename T>
    std::shared_ptr<T> get_service(const std::string& name) {
      for (const auto& child : children)
      {
        if (child->baseName == name)
        {
          return std::dynamic_pointer_cast<T>(child);
        }
      }

      for (const auto& descriptor : ReflectionDescriptorRegistry::instance()->descriptors | std::views::values)
      {
        if (descriptor.name == name)
        {
          return descriptor.construct<T>();
        }
      }

      return nullptr;
    }

  	int get_service_lua(lua_State* context) {
  		const std::string& name = luaL_checkstring(context, -1);
  		auto service = get_service<Instance>(name);

  		if (service != nullptr) {
  			Services::ReflectionService::reflect_class(context, service);
  		} else {
  			luaL_error(context, "No such service exists");
  		}

			return 1;
		}
  };
}
