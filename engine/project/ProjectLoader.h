#include <map>
#include <string>
#include <variant>
#include <vector>

#include "http/json.hpp"
#include "instances/DataModel.h"
#include "instances/Script.h"
#include "lua/system.h"

using namespace Nyanners::Instances;
using SerializedInstanceProperties = std::map<std::string, LuaValue>;

struct SerializedInstanceDescriptor {
    std::string name;
    std::string className;
    SerializedInstanceProperties properties;
    std::vector<SerializedInstanceDescriptor> children;
};
using SerializedInstanceChildren = std::vector<SerializedInstanceDescriptor>;

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

DataModel* loadProjectFile(const std::string projectPath)
{
    DataModel* model = new DataModel();
    std::string project = engine_readFile("system/project.json");
    nlohmann::json projectJson = nlohmann::json::parse(project);

    SerializedInstanceDescriptor root = deserializeInstance(projectJson);
    model->m_name = root.name;

    for (auto child : root.children) {
        if (child.className == "Script") {
            auto filePosition = child.properties.find("file");

            if (filePosition == child.properties.end()) {
                std::cout << "Script is missing file property, which is illegal" << std::endl;
                continue;
            }

            std::string value = std::get<std::string>(filePosition->second);
            Script* script = new Script();
            model->addChild(script);
            script->loadFromFile(value);
        }
    }

    return model;
}