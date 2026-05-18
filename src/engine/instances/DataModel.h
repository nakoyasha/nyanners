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

      for (const auto& descriptor : ReflectionDescriptorRegistry::instance()->descriptors | std::views::values)
      {
        if (descriptor.name == name)
        {
          return descriptor.construct<T>();
        }
      }

      return nullptr;
    }
  };
}
