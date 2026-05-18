#pragma once
#include "ReflectionProperty.h"
#include "ReflectionTypes.h"
#include "lua.h"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace Nyanners::Scripting::Reflection {
	// anyone is welcome to come up with a better idea to do this. i'm tired
	struct ReflectionDescriptorInstance {
		std::function<std::shared_ptr<void>()> constructor = []() -> std::shared_ptr<void> {
			throw std::runtime_error("This instance is not creatable.");
		};
	};

	class ReflectionDescriptor {
	public:
		std::string name;
		ReflectionDescriptorInstance constructorInstance;
		const uint8_t flags {};

		std::vector<ReflectionProperty> properties;
		std::vector<ReflectionProperty> methods;
		std::vector<ReflectionDescriptor*> parents;

		template <typename object, typename T, T (object::*getter)() const, void (object::*setter)(T)>
		ReflectionProperty add_property(const std::string& propertyName, const ReflectionPropertyType type)
		{
			ReflectionProperty property {
				.name = std::move(propertyName),
				.type = type,
				.flags = 0
			};

			property.get = [](void* instance, ReflectionValue& refValue, lua_State* context)
			{
				auto *obj = static_cast<object *>(instance);
				const auto value = (obj->*getter)();
				refValue = value;
			};
			property.set = [](void* instance, const ReflectionValue& value, lua_State* context)
			{
				auto* obj = static_cast<object*>(instance);

				if (const auto castValue = std::get_if<T>(&value))
				{
					(obj->*setter)(*castValue);
				} else
				{
					throw std::invalid_argument("Attempt to set value to an invalid type");
				}
			};

			properties.push_back(property);

			return property;
		};

		template <typename object, typename T, T (object::*getter)() const, void (object::*setter)(T)>
		ReflectionDescriptor& add_property_chained(const std::string& propertyName, const ReflectionPropertyType type)
		{
			add_property<object, T, getter, setter>(propertyName, type);
			return *this;
		};

		template <typename T>
		[[nodiscard]] std::shared_ptr<T> construct() const
		{
			return std::static_pointer_cast<T>(this->constructorInstance.constructor());
		}

		template <class object>
		ReflectionDescriptor& add_constructor() {
			constructorInstance.constructor = []() {
				return std::make_shared<object>();
			};

			return *this;
		}

		std::optional<ReflectionProperty>
		get_property(const std::string &propertyName) const;
	};
}