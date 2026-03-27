#pragma once
#include "Instance.h"
#include <memory>
#include <ranges>

#include "services/ReflectionService.h"

namespace Nyanners::Instances {
  class DataModel : public Instance {
  public:
    DataModel() : Instance("DataModel") {};

    template <typename T>
    std::shared_ptr<T> get_service(const std::string& name) {
      for (const auto& child : children)
      {
        if (child->baseName == name)
        {
          return std::dynamic_pointer_cast<T>(child);
        }
      }

      for (const auto& descriptor : Services::ReflectionService::classes | std::views::values)
      {
        if (descriptor.className == name)
        {
          const auto newInstance = descriptor.constructor();
          return std::dynamic_pointer_cast<T>(newInstance);
        }
      }

      return nullptr;
    }
  };
}
