#pragma once
#include "lua.h"
#include "basic/Object.h"
#include <memory>
#include <utility>
#include <vector>

namespace Nyanners::Instances {
	class Drawable;

	class Instance : public Object,
	                 public std::enable_shared_from_this<Instance> {
	public:
		explicit Instance(std::string name) : Object(std::move(name)) {};

		virtual ~Instance();
		;
		std::weak_ptr<Instance> parent;

		std::vector<std::shared_ptr<Instance>> children;
		std::vector<std::weak_ptr<Drawable>> renderableChildren;

		virtual void add_child(const std::shared_ptr<Instance> &child);
		template <typename... Children>
		void add_child(
		  const std::shared_ptr<Instance> &child, const Children &...children
		) {
			add_child(child);
			(add_child(children), ...);
		}
		virtual void remove_child(const std::shared_ptr<Instance> &child);
		virtual void update(const float deltaTime);

		bool get_active() const {
			return this->active;
		};

		std::shared_ptr<Instance> clone();
		std::shared_ptr<Object> clone_lua();
		int destroy_lua(lua_State *context);

		template <typename T>
		std::shared_ptr<T> find_first_child(const std::string &childName) const {
			for (auto &child : children) {
				if (child->name == childName) {
					return std::dynamic_pointer_cast<T>(child);
				}
			}

			return nullptr;
		}

		template <typename T>
		std::vector<std::weak_ptr<T>>
		peek_at(const std::string &className, const int expectedSize = 0) const {
			std::vector<std::weak_ptr<T>> peek;

			if (expectedSize != 0) {
				peek.reserve(expectedSize);
			}

			for (auto &child : children) {
				if (child->name == className) {
					peek.push_back(std::dynamic_pointer_cast<T>(child));
				}
			}

			return peek;
		}

		std::shared_ptr<Instance> get_parent() const {
			return this->parent.lock();
		};

		std::shared_ptr<Object> get_parent_object() const {
			return std::static_pointer_cast<Object>(this->parent.lock());
		}

		void set_parent(const std::shared_ptr<Instance> &parent) {
			parent->add_child(
			  std::enable_shared_from_this<Instance>::shared_from_this()
			);
		}

		void set_parent_object(const std::shared_ptr<Object> &parent) {
			if (
			  const auto instanceParent = std::dynamic_pointer_cast<Instance>(parent)
			) {
				set_parent(instanceParent);
			}
		}
	};
}
