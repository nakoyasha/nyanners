#pragma once
#include <string>

#include "Aliases.h"
#include "instances/DataModel.h"

namespace Nyanners::Core {
	class Project {
	public:
		Project(const std::string& pId, const std::string &pName, const std::string& pInitScene) : id(std::move(pId)), name(std::move(pName)), initScene(std::move(pInitScene)) {}

		std::string id;
		std::string name;
		std::string initScene;
		// TODO: project settings
		int settings = -1;

		Ref<DataModel> load_init_scene() const;
	};
}
