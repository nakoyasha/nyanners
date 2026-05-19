#pragma once
#include "ReflectionProperty.h"
#include "instances/Instance.h"

namespace Nyanners::Scripting::Reflection {
	using ReflectionMethodCallback = int (*)(Instances::Instance* instance, lua_State* context);

	struct ReflectionMethod {
		const std::string name;
		const ReflectionPropertyType returnType;
		const uint8_t flags;

		ReflectionMethodCallback call;
	};
}