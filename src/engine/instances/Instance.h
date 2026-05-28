#pragma once
#include "drawable/Drawable.h"
#include <memory>
#include "lua.h"
#include <utility>
#include <vector>

#include "core/Logger.h"

namespace Nyanners::Instances {
  class Instance : public std::enable_shared_from_this<Instance> {
    public:
    const std::string baseName;
    std::string name;
    bool active = true;

    explicit Instance(std::string name) : baseName(std::move(name)), name(baseName) {};

    virtual ~Instance();;
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

  	[[nodiscard]] std::string get_name() const;
  	[[nodiscard]] std::string get_basename() const;

  	void set_name(const std::string& name) {
  		this->name = name;
  	}

    virtual void set_active(const bool newActiveState);
  	std::shared_ptr<Instance> clone();
  	int clone_lua(lua_State* context);
  	int destroy_lua(lua_State* context);

    template <typename T>
    std::shared_ptr<T> find_first_child(const std::string& childName) const {
      for (auto& child : children) {
        if (child->name == childName) {
          return std::dynamic_pointer_cast<T>(child);
        }
      }

      return nullptr;
    }

  	template <typename T>
  	std::vector<std::weak_ptr<T>> peek_at(const std::string& className, const int expectedSize = 0) const {
	    std::vector<std::weak_ptr<T>> peek;

    	if (expectedSize != 0) {
    		peek.reserve(expectedSize);
    	}

    	for (auto& child : children) {
    		if (child->name == className) {
    			peek.push_back(std::dynamic_pointer_cast<T>(child));
    		}
    	}

    	return peek;
    }


  	std::shared_ptr<Instance> get_parent() const {
	    return this->parent.lock();
    };

  	void set_parent(const std::shared_ptr<Instance>& parent) {
  		parent->add_child(shared_from_this());
  	}
  };
}
