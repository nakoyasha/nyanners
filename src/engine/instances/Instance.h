#pragma once
#include "lua.h"
#include "basic/Object.h"
#include "core/Aliases.h"
#include <memory>
#include <utility>
#include <vector>

namespace Nyanners::Instances {
	class Drawable;

	class Instance : public Object, public std::enable_shared_from_this<Instance> {
	public:
		explicit Instance(std::string name) : Object(std::move(name)) {};
		~Instance() override;

		TempRef<Instance> parent;

		List<Ref<Instance>> children;
		List<TempRef<Drawable>> renderableChildren;

		void update(float deltaTime) override;
		Ref<Instance> clone();
		Ref<Object> clone_lua();
		int destroy_lua(lua_State *context);

		virtual void add_child(const Ref<Instance> &child);
		virtual void remove_child(const Ref<Instance> &child);
		template <typename... Children>
		void add_child(const Ref<Instance> &child, const Children &...children) {
			add_child(child);
			(add_child(children), ...);
		}

		template <typename T>
		Ref<T> find_first_child(const std::string &childName) const {
			for (auto &child : children) {
				if (child->name == childName) {
					return std::dynamic_pointer_cast<T>(child);
				}
			}

			return nullptr;
		}

		template <typename T>
		List<TempRef<T>> peek_at(const std::string &className, const int expectedSize = 0) const {
			List<TempRef<T>> peek;

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

		Ref<Instance> get_parent() const {
			return this->parent.lock();
		};

		Ref<Object> get_parent_object() const {
			return std::static_pointer_cast<Object>(this->parent.lock());
		}

		List<Ref<Instance>> get_descendants();

		void set_parent(const Ref<Instance> &newParent) {
			newParent->add_child(shared_from_this());
		}

		void set_parent_object(const Ref<Object> &newParent) {
			if (const auto instanceParent = std::dynamic_pointer_cast<Instance>(newParent)) {
				set_parent(instanceParent);
			}
		}
	private:
		static void fill_descendants_vector(List<Ref<Instance>> &descendants, const Ref<Instance> &nextInstance);
	};
}
