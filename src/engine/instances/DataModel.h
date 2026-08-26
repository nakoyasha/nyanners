#pragma once
#include "Instance.h"
#include <memory>
#include <ranges>

#include "lualib.h"
#include "core/Logger.h"
#include "core/ServiceProvider.h"
#include "services/ReflectionService.h"

namespace Nyanners {
	class Application;
}

namespace Nyanners::Instances {
  class DataModel : public Instance {
  public:
    DataModel();

  	int test_lua_method(lua_State* context) {
  		lua_pushboolean(context, true);
  		return 1;
  	}

    template <typename T>
    std::shared_ptr<T> get_service(const std::string& name) {

  		if (name != "ConsoleService") {
	      for (const auto& child : children)
	      {
	        if (child->baseName == name)
	        {
	          return std::dynamic_pointer_cast<T>(child);
	        }
	      }
  		}

  		try {
  			return Services::ServiceProvider::instance()->get_service<T>(name);
  		} catch (std::runtime_error& e) {
  			Core::Logger::log_error(e.what());
  		}

      for (const auto& descriptor : ReflectionDescriptorRegistry::instance()->descriptors | std::views::values)
      {
        if (descriptor.name == name)
        {
          return std::dynamic_pointer_cast<T>(descriptor.construct());
        }
      }
  		return nullptr;
    }

  	int get_service_lua(lua_State* context) {
  		const std::string& name = luaL_checkstring(context, -1);

  		try {
  			const auto service = get_service<Object>(name);

  			if (service != nullptr) {
  				Services::ReflectionService::reflect_class(context, service);
  				return 1;
  			} else {
  				luaL_error(context, "No such service exists");
  			}
  		} catch (std::runtime_error& e) {
  			luaL_error(context, e.what());
  		}

	    return 0;
  	}
  };
}
