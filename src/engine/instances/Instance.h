#pragma once
#include <iostream>
#include <utility>
#include <vector>
#include <memory>

namespace Nyanners::Instances {
  class Instance : public std::enable_shared_from_this<Instance> {
    public:
    std::string name;
    explicit Instance(std::string name) : baseName(std::move(name)) {
      this->name = this->baseName;
    };

    virtual ~Instance() = default;
    std::shared_ptr<Instance> parent;

    const std::string baseName;
    std::vector<std::shared_ptr<Instance>> children;

    void add_child(const std::shared_ptr<Instance> &child);
    void remove_child(const std::shared_ptr<Instance> &child);

    template <typename T>
    std::shared_ptr<T> find_first_child(const std::string& childName) const {
      for (auto& child : children) {
        if (child->name == childName) {
          return std::dynamic_pointer_cast<T>(child);
        }
      }

      return nullptr;
    }

  };
} // namespace Instances