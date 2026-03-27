#pragma once
#include "Instance.h"
#include <memory>

namespace Nyanners::Instances {
  class DataModel : public Instance {
  public:
    DataModel() : Instance("DataModel") {};

    template <typename T>
    std::shared_ptr<T> get_service(const std::string& name) {
      return std::dynamic_pointer_cast<T>();
    }
  };
}