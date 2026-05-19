#pragma once
#include "ReflectionMethod.h"
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
		std::vector<ReflectionMethod> methods;
		std::vector<ReflectionDescriptor*> parents;

		template <typename object, typename T, T (object::*getter)() const, void (object::*setter)(const T&)>
		ReflectionProperty add_property(const std::string& propertyName, const ReflectionPropertyType type)
		{
			ReflectionProperty property {
				.name = propertyName,
				.type = type,
				.flags = 0
			};

			property.get = [](Instances::Instance* instance, ReflectionValue& refValue, lua_State* context)
			{
				auto *obj = dynamic_cast<object *>(instance);
				const auto value = (obj->*getter)();
				refValue = value;
			};

			property.set = [](Instances::Instance* instance, const ReflectionValue& value, lua_State* context)
			{
				auto* obj = dynamic_cast<object*>(instance);

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

		template <typename object, typename T, T (object::*getter)() const, void (object::*setter)(const T)>
		ReflectionProperty add_property(const std::string& propertyName, const ReflectionPropertyType type)
		{
			ReflectionProperty property {
				.name = propertyName,
				.type = type,
				.flags = 0
			};

			property.get = [](Instances::Instance* instance, ReflectionValue& refValue, lua_State* context)
			{
				auto *obj = dynamic_cast<object *>(instance);
				const auto value = (obj->*getter)();
				refValue = value;
			};

			property.set = [](Instances::Instance* instance, const ReflectionValue& value, lua_State* context)
			{
				auto* obj = dynamic_cast<object*>(instance);

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

		template <typename object, typename T, T (object::*getter)() const>
		ReflectionProperty add_property(const std::string& propertyName, const ReflectionPropertyType type)
		{
			ReflectionProperty property {
				.name = propertyName,
				.type = type,
				.flags = 0
			};

			property.get = [](Instances::Instance* instance, ReflectionValue& refValue, lua_State* context)
			{
				auto *obj = dynamic_cast<object *>(instance);
				const auto value = (obj->*getter)();
				refValue = value;
			};

			property.set = [](Instances::Instance* instance, const ReflectionValue& value, lua_State* context)
			{
				throw std::logic_error("you absolute buffon. this is a read-only property what the fuck are you trying to do");
			};

			properties.push_back(property);

			return property;
		};

		template <typename object, typename T, T (object::*getter)() const, void (object::*setter)(const T&)>
		ReflectionDescriptor& add_property_chained(const std::string& propertyName, const ReflectionPropertyType type)
		{
			add_property<object, T, getter, setter>(propertyName, type);
			return *this;
		};

		template <typename object, typename T, T (object::*getter)() const, void (object::*setter)(const T)>
		ReflectionDescriptor& add_property_chained(const std::string& propertyName, const ReflectionPropertyType type)
		{
			add_property<object, T, getter, setter>(propertyName, type);
			return *this;
		};

		template <typename object, typename T, T (object::*getter)() const>
		ReflectionDescriptor& add_property_chained(const std::string& propertyName, const ReflectionPropertyType type)
		{
			add_property<object, T, getter>(propertyName, type);
			return *this;
		};

		template <typename object, int (object::*method)(lua_State* context)>
		ReflectionMethod add_method(const std::string& methodName, const ReflectionPropertyType returnType)
		{
			ReflectionMethod methodObject {
				.name = methodName,
				.returnType = returnType,
				.flags = 0
			};

			methodObject.call = [](Instances::Instance* instance, lua_State* context) -> int
			{
				auto *obj = dynamic_cast<object*>(instance);
				// TODO: make this accept variadic arguments
				return (obj->*method)(context);
			};

			methods.push_back(methodObject);
			return methodObject;
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

		[[nodiscard]] std::optional<ReflectionProperty> get_property(const std::string &propertyName) const;
		[[nodiscard]] std::optional<ReflectionMethod> get_method(const std::string &propertyName) const;
	};
}