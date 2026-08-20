#pragma once
#include "ReflectionProperty.h"
#include "instances/basic/Object.h"
#include <vector>

namespace Nyanners::Scripting::Reflection {
	struct ReflectionMethodParameter {
		std::string name;
		ReflectionPropertyType type;
	};

	using ReflectionMethodCallback =
	  int (*)(Instances::Object *instance, lua_State *context);

	struct ReflectionMethod {
		const std::string name;
		const ReflectionPropertyType returnType;
		const uint8_t flags;
		std::vector<ReflectionMethodParameter> parameters{};

		ReflectionMethodCallback call;
	};
}