#include "ProjectParser.h"

#include "instances/services/UIService.h"
#include "instances/world/World.h"

using namespace Nyanners::Serialization;
using namespace Nyanners::Core;

Ref<DataModel> ProjectParser::make_blank_data_model() {
	const auto model = std::make_shared<DataModel>();

	model->add_child(std::make_shared<Services::SelectionService>());
	model->add_child(std::make_shared<Services::UIService>());
	model->add_child(std::make_shared<Services::World>());
	model->add_child(std::make_shared<Services::ScriptService>());
	model->add_child(std::make_shared<Services::IOService>());

	return model;
}

Project ProjectParser::load_project_from_file(const std::filesystem::path &path) {
	const auto object = nlohmann::json::parse(Services::IOService::instance()->read_file(path));
	return deserialize_project(object);
}

Project ProjectParser::deserialize_project(const nlohmann::json &projectObject) {
	const auto io = Services::IOService::instance();

	// TODO: parse settings here
	// const auto projectSettings = projectObject.at("settings").items();
	const auto id = projectObject.at("id").get<std::string>();
	const auto name = projectObject.at("name").get<std::string>();
	const auto initial_scene = projectObject.at("initial_scene").get<std::string>();

	if (io->file_exists(initial_scene)) {
		auto object = nlohmann::json::parse(io->read_file(initial_scene));

	}

	return Project(id, name, initial_scene);
}

Ref<Nyanners::Instances::Instance> ProjectParser::deserialize_instance(const nlohmann::json &object) {
	const auto registry = ReflectionDescriptorRegistry::instance();

	if (!object.contains("type") || !object.contains("properties")) {
		Logger::log_error("Object to deserialize has no type or properties");
		return nullptr;
	}

	const std::string type = object["type"].get<std::string>();
	const auto& properties = object["properties"];
	const auto& children = object["children"];

	const auto descriptorIndex = registry->descriptors.find(type);

	if (descriptorIndex == registry->descriptors.end()) {
		Logger::log_error(std::format("Failed to deserialize {}: No such object is registered", type));
		return nullptr;
	}

	const auto descriptor = descriptorIndex->second;
	const auto instance = std::dynamic_pointer_cast<Instances::Instance>(descriptor.construct());

	hydrate_instance(instance, descriptor, properties);

	for (const auto& child : children) {
		instance->add_child(deserialize_instance(child));
	}

	return instance;
}

Ref<DataModel> ProjectParser::deserialize_scene(const std::filesystem::path &path) {
	auto model = make_blank_data_model();
	const auto registry = ReflectionDescriptorRegistry::instance();

	const auto json = nlohmann::json::parse(Services::IOService::instance()->read_file(path));
	const auto& items = json.at("model").items();

	for (const auto& [key, value] : items) {
		const auto type = value.at("type");
		const auto descriptorIndex = registry->descriptors.find(type);

		if (descriptorIndex == registry->descriptors.end()) {
			Logger::log_error(std::format("Failed to locate {}", type));
			continue;
		}

		const auto descriptor = descriptorIndex->second;
		const auto properties = value.at("properties");

		if (descriptor.has_flag(ReflectionInstanceFlags::Service)) {
			// this should create and / or fetch the service
			auto service = model->get_service<Instances::Instance>(descriptor.name);
			const auto children = value.at("children");
			hydrate_instance(service, descriptor, properties);

			for (const auto child : children) {
				auto object = deserialize_instance(child);
				service->add_child(object);
			}
		} else {
			auto object = deserialize_instance(value);
			model->add_child(object);
		}
	}
	return model;
}

void ProjectParser::hydrate_instance(const Ref<Instances::Instance> &instance, const ReflectionDescriptor &descriptor, const nlohmann::json &propertiesObject) {
	const auto& properties = propertiesObject.items();

	for (const auto& [key, value] : properties) {
		try {
			const auto property = descriptor.get_property(key);

			if (property == std::nullopt) {
				throw std::runtime_error("Unknown property");
			}

			// special case
			if (property->name == "Material") {
				Logger::log_error("Material deserialization is not yet implemented");
				continue;
			}

			if (property->type == String) {
				property->set(instance.get(), value.get<std::string>(), nullptr);
			} else if (property->type == Boolean) {
				property->set(instance.get(), value.get<bool>(), nullptr);
			} else if (property->type == Number) {
				property->set(instance.get(), value.get<double>(), nullptr);
			} else if (property->type == Integer) {
				property->set(instance.get(), value.get<int>(), nullptr);
			} else if (property->type == Vector3) {
				const auto x = value.at("x").get<float>();
				const auto y = value.at("y").get<float>();
				const auto z = value.at("z").get<float>();

				property->set(instance.get(), glm::vec3(x, y, z), nullptr);
			} else if (property->type == Vector2) {

				if (!value.is_object()) {
					throw std::runtime_error("Vector2 property: expected JSON object, got.. something else");
				}

				if (!value.contains("x")) {
					throw std::runtime_error("Vector2 property: expected JSON object to have x value");
				}

				if (!value.contains("y")) {
					throw std::runtime_error("Vector2 property: expected JSON object to have y value");
				}

				const auto x = value.at("x").get<float>();
				const auto y = value.at("y").get<float>();

				property->set(instance.get(), glm::vec2(x, y), nullptr);
			}
		} catch (std::runtime_error err) {
			Logger::log_error(std::format("Failed to set property {}: {}", key, err.what()));
		}
	}
}
