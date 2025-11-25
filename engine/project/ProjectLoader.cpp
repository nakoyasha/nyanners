#include "ProjectLoader.h"

SerializedInstanceProperties deserializeInstanceProperties(nlohmann::json serializedProperties)
{
    SerializedInstanceProperties properties {};

    for (auto [key, value] : serializedProperties.items()) {
        if (value.is_string()) {
            properties.insert_or_assign(key, value.get<std::string>());
        } else if (value.is_number_integer()) {
            properties.insert_or_assign(key, value.get<int>());
        } else if (value.is_number_float()) {
            properties.insert_or_assign(key, value.get<float>());
        }
    }

    return properties;
}

SerializedInstanceDescriptor deserializeInstance(nlohmann::json serializedInstance)
{
    std::string name = "Instance";
    std::string className = "Instance";

    SerializedInstanceProperties properties;
    SerializedInstanceChildren children;

    for (auto [key, value] : serializedInstance.items()) {
        if (key == "name") {
            name = value;
        } else if (key == "className") {
            className = value;
        } else if (key == "properties") {
            properties = deserializeInstanceProperties(value);
        } else if (key == "children") {
            for (auto [_, child] : value.items()) {
                SerializedInstanceDescriptor instance = deserializeInstance(child);
                children.push_back(instance);
            }
        }
    }

    SerializedInstanceDescriptor descriptor;
    descriptor.name = name;
    descriptor.className = className;
    descriptor.properties = properties;
    descriptor.children = children;

    return descriptor;
}