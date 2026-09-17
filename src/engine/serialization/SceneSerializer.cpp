#include "SceneSerializer.h"

using namespace Nyanners::Serialization;
using namespace Nyanners::Core;

nlohmann::json SceneSerializer::serialize_scene(const std::string& name, const Ref<DataModel> &model) {
	nlohmann::json result;
	result["id"] = name;
	result["settings"] = {};

	std::vector<nlohmann::json> children;

	for (const auto& child : model->children) {
		auto serialized = serialize_object(child);
		children.push_back(serialized);
	}

	result["model"] = children;

	return result;
}

nlohmann::json SceneSerializer::serialize_object(const Ref<Object> &object) {
	const auto registry = ReflectionDescriptorRegistry::instance();
	nlohmann::json result;

	std::map<std::string, nlohmann::json> properties;
	std::vector<nlohmann::json> children;

	const auto descriptorIndex = registry->descriptors.find(object->baseName);

	result["type"] = object->baseName;
	if (descriptorIndex != registry->descriptors.end()) {
		const auto descriptor = descriptorIndex->second;
		bool inheritsFromInstance = false;

		for (const auto& parent : descriptor.parents) {
			if (parent->name == "Instance") {
				inheritsFromInstance = true;
				break;
			}
		}

		for (const auto& property : Services::ReflectionService::get_properties(object)) {
			if (property.has_flag(ReflectionPropertyFlags::NotSerializable)) {
				continue;
			}

			ReflectionValue value;
			property.get(object.get(), value, nullptr);

			if (property.type == String) {
				properties[property.name] = std::get<std::string>(value);
			} else if (property.type == Boolean) {
				properties[property.name] = std::get<bool>(value);
			} else if (property.type == Number) {
				properties[property.name] = std::get<double>(value);
			} else if (property.type == Integer) {
				properties[property.name] = std::get<int>(value);
			} else if (property.type == Vector3) {
				auto vector = std::get<glm::vec3>(value);
				properties[property.name] = {{"x", vector.x}, {"y", vector.y}, {"z", vector.z}};
			} else if (property.type == Vector2) {
				auto vector = std::get<glm::vec2>(value);
				properties[property.name] = {{"x", vector.x}, {"y", vector.y}};
			} else if (property.type == Material_) {
				Logger::log_debug("TODO: Implement Material_");
			}
		}

		if (inheritsFromInstance) {
			const auto instance = std::dynamic_pointer_cast<Instances::Instance>(object);

			for (const auto& child : instance->children) {
				children.push_back(serialize_object(child));
			}
		}
	}

	result["children"] = children;
	result["properties"] = properties;

	return result;
}