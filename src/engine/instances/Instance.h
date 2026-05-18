#pragma once
#include "drawable/Drawable.h"
#include <memory>
#include <utility>
#include <vector>

namespace Nyanners::Instances {
  class Instance : public std::enable_shared_from_this<Instance> {
    public:
    const std::string baseName;
    std::string name;
    bool active = true;

    explicit Instance(std::string name) : baseName(std::move(name)), name(baseName) {};

    virtual ~Instance() = default;
    std::weak_ptr<Instance> parent;

    std::vector<std::shared_ptr<Instance>> children;
  	std::vector<std::weak_ptr<Drawable>> renderableChildren;

    virtual void add_child(const std::shared_ptr<Instance>& child);
  	template <typename... Children>
		void add_child(const std::shared_ptr<Instance>& child, const Children&... children)
  	{
  		add_child(child);
  		(add_child(children), ...);
  	}
    virtual void remove_child(const std::shared_ptr<Instance> &child);
    virtual void update(const float deltaTime);

  	bool get_active() const {
  		return this->active;
  	};

  	[[nodiscard]] std::string get_name() const {
  		return this->name;
  	};

  	void set_name(std::string name) {
  		this->name = name;
  	}


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