#pragma once
#include "instances/datatypes/Color3.h"
#include "instances/services/ReflectionService.h"

namespace Nyanners::Scripting::Reflection {
	int push_color3(lua_State* context, DataTypes::Color3& color);
	int push_vector3 (lua_State* context, glm::vec3& color);
}