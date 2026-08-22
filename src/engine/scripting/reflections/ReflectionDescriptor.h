#pragma once
#include "ReflectionMethod.h"
#include "ReflectionMethodTraits.h"
#include "ReflectionProperty.h"
#include "ReflectionPropertyReaders.h"
#include "ReflectionTypes.h"
#include "lua.h"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace Nyanners::Services {
	class ReflectionService;
}

namespace Nyanners::Scripting::Reflection {
	// anyone is welcome to come up with a better idea to do this. i'm tired
	struct ReflectionDescriptorInstance {
		std::function<std::shared_ptr<Instances::Object>()> constructor =
		  []() -> std::shared_ptr<Instances::Object> {
			throw std::runtime_error("This instance is not creatable.");
		};
	};

	class ReflectionDescriptor {
	public:
		std::string name;
		ReflectionDescriptorInstance constructorInstance;
		uint8_t flags{};

		std::vector<ReflectionProperty> properties;
		std::vector<ReflectionMethod> methods;
		std::vector<ReflectionDescriptor *> parents;
		std::vector<std::string> pending_parents{};

		template <
		  typename object,
		  typename T,
		  T (object::*getter)() const,
		  void (object::*setter)(const T &)>
		ReflectionProperty add_property(
		  const std::string &propertyName, const ReflectionPropertyType type
		) {
			ReflectionProperty property{
			  .name = propertyName, .type = type, .flags = 0
			};

			property.get = [](
			                 Instances::Object *instance,
			                 ReflectionValue &refValue,
			                 lua_State *context
			               ) {
				auto *obj = dynamic_cast<object *>(instance);
				const auto value = (obj->*getter)();
				refValue = value;
			};

			property.set = [](
			                 Instances::Object *instance,
			                 const ReflectionValue &value,
			                 lua_State *context
			               ) {
				auto *obj = dynamic_cast<object *>(instance);

				if (const auto castValue = std::get_if<T>(&value)) {
					(obj->*setter)(*castValue);
				} else {
					throw std::invalid_argument(
					  "Attempt to set value to an invalid type"
					);
				}
			};

			properties.push_back(property);

			return property;
		};

		template <
		  typename object,
		  typename T,
		  T (object::*getter)() const,
		  void (object::*setter)(const T)>
		ReflectionProperty add_property(
		  const std::string &propertyName, const ReflectionPropertyType type
		) {
			ReflectionProperty property{
			  .name = propertyName, .type = type, .flags = 0
			};

			property.get = [](
			                 Instances::Object *instance,
			                 ReflectionValue &refValue,
			                 lua_State *context
			               ) {
				auto *obj = dynamic_cast<object *>(instance);
				const auto value = (obj->*getter)();
				refValue = value;
			};

			property.set = [](
			                 Instances::Object *instance,
			                 const ReflectionValue &value,
			                 lua_State *context
			               ) {
				auto *obj = dynamic_cast<object *>(instance);

				if (const auto castValue = std::get_if<T>(&value)) {
					(obj->*setter)(*castValue);
				} else {
					throw std::invalid_argument(
					  "Attempt to set value to an invalid type"
					);
				}
			};

			properties.push_back(property);

			return property;
		};

		template <typename object, typename T, T (object::*getter)() const>
		ReflectionProperty add_property(
		  const std::string &propertyName, const ReflectionPropertyType type
		) {
			ReflectionProperty property{
			  .name = propertyName, .type = type, .flags = 0
			};
			property.flags ^= static_cast<uint8_t>(ReflectionPropertyFlags::ReadOnly);

			property.get = [](
			                 Instances::Object *instance,
			                 ReflectionValue &refValue,
			                 lua_State *context
			               ) {
				auto *obj = dynamic_cast<object *>(instance);
				const auto value = (obj->*getter)();
				refValue = value;
			};

			property.set = [](
			                 Instances::Object *instance,
			                 const ReflectionValue &value,
			                 lua_State *context
			               ) {
				throw std::logic_error(
				  "you absolute buffon. this is a read-only property what the fuck are you trying to do"
				);
			};

			properties.push_back(property);

			return property;
		};

		template <
		  typename object,
		  typename T,
		  T (object::*getter)() const,
		  void (object::*setter)(const T &)>
		ReflectionDescriptor &add_property_chained(
		  const std::string &propertyName, const ReflectionPropertyType type
		) {
			add_property<object, T, getter, setter>(propertyName, type);
			return *this;
		};

		template <
		  typename object,
		  typename T,
		  T (object::*getter)() const,
		  void (object::*setter)(const T &)> 
		ReflectionDescriptor &add_enum_property_chained(
		  const std::string &propertyName, const std::string &enumName
		) {
			static_assert(std::is_enum_v<T>);
			ReflectionProperty property{
			  .name = propertyName,
			  .type = ReflectionPropertyType::Enum,
			  .flags = 0,
			  .enumName = enumName
			};

			property.get = [](Instances::Object *instance, ReflectionValue &value,
			                  lua_State *) {
				auto *obj = dynamic_cast<object *>(instance);
				value = static_cast<int>((obj->*getter)());
			};
			property.set = [](Instances::Object *instance, const ReflectionValue &value,
			                  lua_State *) {
				auto *obj = dynamic_cast<object *>(instance);
				if (const auto enumValue = std::get_if<int>(&value)) {
					(obj->*setter)(static_cast<T>(*enumValue));
					return;
				}
				throw std::invalid_argument("Attempt to set enum to an invalid type");
			};

			properties.push_back(property);
			return *this;
		};

		template <typename object, typename T, T (object::*getter)() const>
		ReflectionDescriptor &add_enum_property_chained(
		  const std::string &propertyName, const std::string &enumName
		) {
			static_assert(std::is_enum_v<T>);
			ReflectionProperty property{
			  .name = propertyName,
			  .type = ReflectionPropertyType::Enum,
			  .flags = static_cast<uint8_t>(ReflectionPropertyFlags::ReadOnly),
			  .enumName = enumName
			};

			property.get = [](Instances::Object *instance, ReflectionValue &value,
			                  lua_State *) {
				auto *obj = dynamic_cast<object *>(instance);
				value = static_cast<int>((obj->*getter)());
			};
			property.set = [](Instances::Object *, const ReflectionValue &, lua_State *) {
				throw std::logic_error("Attempt to set a read-only enum property");
			};

			properties.push_back(property);
			return *this;
		};


		template <
		  typename object,
		  typename T,
		  T (object::*getter)() const,
		  void (object::*setter)(const T)>
		ReflectionDescriptor &add_property_chained(
		  const std::string &propertyName, const ReflectionPropertyType type
		) {
			add_property<object, T, getter, setter>(propertyName, type);
			return *this;
		};

		template <typename object, typename T, T (object::*getter)() const>
		ReflectionDescriptor &add_property_chained(
		  const std::string &propertyName, const ReflectionPropertyType type
		) {
			add_property<object, T, getter>(propertyName, type);
			return *this;
		};

		// old, only rlly used now because it takes a context
		template <typename object, int (object::*method)(lua_State *context)>
		ReflectionDescriptor &add_method(
		  const std::string &methodName, const ReflectionPropertyType returnType
		) {
			ReflectionMethod methodObject{
			  .name = methodName, .returnType = returnType, .flags = 0
			};

			methodObject.call =
			  [](Instances::Object *instance, lua_State *context) -> int {
				auto *obj = dynamic_cast<object *>(instance);
				// TODO: make this accept variadic arguments
				return (obj->*method)(context);
			};

			methods.push_back(methodObject);
			return *this;
		};

		ReflectionDescriptor &add_method_anon(
		  const std::string &methodName,
		  const ReflectionMethodCallback callback,
		  const ReflectionPropertyType returnType
		) {
			ReflectionMethod methodObject{
			  .name = methodName,
			  .returnType = returnType,
			  .flags = 0,
			  .call = callback
			};

			methods.push_back(methodObject);
			return *this;
		};

		[[nodiscard]] std::shared_ptr<Instances::Object> construct() const {
			return this->constructorInstance.constructor();
		}

		template <auto method>
		ReflectionDescriptor &add_method(
		  const std::string &methodName,
		  const ReflectionPropertyType returnType,
		  const std::vector<ReflectionMethodParameter> &parameters = {}
		) {
			ReflectionMethod methodObject{
			  .name = methodName,
			  .returnType = returnType,
			  .flags = 0,
			  .parameters = parameters
			};
			methodObject.call = &ReflectionMethodInvoker<method>::invoke;
			methods.push_back(methodObject);
			return *this;
		}

		template <class object> ReflectionDescriptor &add_constructor() {
			constructorInstance.constructor = []() {
				return std::make_shared<object>();
			};

			return *this;
		}

		[[nodiscard]] std::optional<ReflectionProperty>
		get_property(const std::string &propertyName) const;
		[[nodiscard]] std::optional<ReflectionMethod>
		get_method(const std::string &propertyName) const;
	};
}
