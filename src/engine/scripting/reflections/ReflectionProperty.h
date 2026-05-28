#pragma once
#include "ReflectionTypes.h"
#include "lua.h"
#include "instances/Instance.h"
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
	  std::shared_ptr<Instances::Instance>,
	  Instances::SignalBase*,
	  glm::vec3,
	  glm::vec2,
	  DataTypes::Color3
	>;

	using GetterMethod = void (*)(Instances::Instance *instance, ReflectionValue& refValue, lua_State *context);
	using SetterMethod = void (*)(Instances::Instance* instance, const ReflectionValue& value, lua_State* context);

	struct ReflectionProperty {
		const std::string name;
		const ReflectionPropertyType type;
		// const std::string category = "Unknown";
		uint8_t flags;

		GetterMethod get;
		SetterMethod set;

		[[nodiscard]] bool has_flag(const ReflectionPropertyFlags& flag) const {
			return this->flags & static_cast<uint8_t>(flag);
		};
	};
}