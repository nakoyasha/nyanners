#pragma once
#include "ReflectionTypes.h"
#include "lua.h"
#include "instances/basic/Object.h"
#include "instances/basic/SignalBase.h"
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
	  std::shared_ptr<Instances::Object>,
	  Instances::SignalBase *,
	  glm::vec3,
	  glm::vec2,
	  DataTypes::Color3>;

	using GetterMethod = void (*)(
	  Instances::Object *instance, ReflectionValue &refValue, lua_State *context
	);
	using SetterMethod = void (*)(
	  Instances::Object *instance,
	  const ReflectionValue &value,
	  lua_State *context
	);

	struct ReflectionProperty {
		const std::string name;
		const ReflectionPropertyType type;
		// const std::string category = "Unknown";
		uint8_t flags;
		std::string enumName;

		GetterMethod get;
		SetterMethod set;

		[[nodiscard]] bool has_flag(const ReflectionPropertyFlags &flag) const {
			return this->flags & static_cast<uint8_t>(flag);
		};
	};
}