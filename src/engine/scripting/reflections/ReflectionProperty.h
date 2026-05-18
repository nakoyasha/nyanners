#pragma once
#include "ReflectionTypes.h"
#include "lua.h"
#include "instances/Instance.h"
#include <string>
#include <variant>

namespace Nyanners::Scripting::Reflection {
	using ReflectionValue = std::variant<
	  std::monostate, // void
	  bool,
	  int,
	  double,
	  float,
	  std::string,
	  std::shared_ptr<Instances::Instance>,
	  glm::vec3,
	  glm::vec2,
		DataTypes::Color3
	>;

	using GetterMethod = void (*)(void *instance, ReflectionValue& refValue, lua_State *context);
	using SetterMethod = void (*)(void* instance, const ReflectionValue& value, lua_State* context);

	struct ReflectionProperty {
		const std::string name;
		const ReflectionPropertyType type;
		// const std::string category = "Unknown";
		const uint8_t flags;

		GetterMethod get;
		SetterMethod set;
	};
}