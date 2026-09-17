#pragma once
#include "instances/DataModel.h"
#include "instances/basic/Object.h"
#include "nlohmann/json.hpp"

namespace Nyanners::Serialization {
	class SceneSerializer {
		public:

		static nlohmann::json serialize_scene(const std::string& name, const Ref<DataModel>& model);
		static nlohmann::json serialize_object(const Ref<Object>& object);
	};
}
