#pragma once
#include <map>
#include <string>
#include <variant>
#include <vector>

#include "http/json.hpp"
#include "lua/reflection/ReflectionTypes.h"

using SerializedInstanceProperties = std::map<std::string, LuaValue>;

struct SerializedInstanceDescriptor {
    std::string name;
    std::string className;
    SerializedInstanceProperties properties;
    std::vector<SerializedInstanceDescriptor> children;
};
using SerializedInstanceChildren = std::vector<SerializedInstanceDescriptor>;

SerializedInstanceProperties deserializeInstanceProperties(nlohmann::json serializedProperties);
SerializedInstanceDescriptor deserializeInstance(nlohmann::json serializedInstance);