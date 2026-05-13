#pragma once
#include "instances/datatypes/Color3.h"
#include "instances/services/ReflectionService.h"

namespace Nyanners::Scripting::Reflection {
	int push_color3(lua_State *context, DataTypes::Color3 &color);
	int push_vector3 (lua_State* context, glm::vec3& vector);
	int push_vector2 (lua_State* context, glm::vec2& vector);

	glm::vec3* get_vector3_from_lua(lua_State* context, int idx = -1);
	glm::vec2* get_vector2_from_lua(lua_State* context, int idx = -1);
}