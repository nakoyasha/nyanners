#pragma once
#include "drawable/Drawable.h"
#include <memory>
#include <utility>
#include <vector>

namespace Nyanners::Instances {
  class Instance : public std::enable_shared_from_this<Instance> {
    public:
    std::string name;
    bool active = true;

    explicit Instance(std::string name) : baseName(std::move(name)) {
      this->name = this->baseName;
    };

    virtual ~Instance() {
	    for (const auto child : children) {
		    Instance::remove_child(child);
	    	delete child.get();
	    }
    	children.clear();
    };
    std::shared_ptr<Instance> parent;

    const std::string baseName;
    std::vector<std::shared_ptr<Instance>> children;
  	std::vector<std::shared_ptr<Drawable>> renderableChildren;

    virtual void add_child(const std::shared_ptr<Instance>& child);
  	template <typename... Children>
		void add_child(const std::shared_ptr<Instance>& child, const Children&... children)
  	{
  		add_child(child);
  		(add_child(children), ...);
  	}
    virtual void remove_child(const std::shared_ptr<Instance> &child);

    virtual void update(const float deltaTime);
    virtual void set_active(const bool newActiveState);

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