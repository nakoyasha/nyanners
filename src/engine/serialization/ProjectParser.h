#pragma once
#include "core/Aliases.h"
#include "core/Project.h"
#include "instances/DataModel.h"
#include "instances/services/SelectionService.h"
#include "instances/services/io/IOService.h"

#include <nlohmann/json.hpp>

namespace Nyanners::Serialization {
	class ProjectParser {
		public:

		static Ref<DataModel> make_blank_data_model();
		static Core::Project load_project_from_file(const std::filesystem::path& path);
		static Core::Project deserialize_project(const nlohmann::json& projectObject);
		static Ref<Instances::Instance> deserialize_instance(const nlohmann::json& object);
		static Ref<DataModel> deserialize_scene(const std::filesystem::path& path);
		static void hydrate_instance(const Ref<Instances::Instance>& instance, const ReflectionDescriptor& descriptor, const nlohmann::json& propertiesObject);
	};
}
