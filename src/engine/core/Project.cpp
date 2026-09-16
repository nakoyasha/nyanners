#include "Project.h"

#include "serialization/ProjectParser.h"

using namespace Nyanners::Core;

Ref<DataModel> Project::load_init_scene() const {
	return Serialization::ProjectParser::deserialize_scene(this->initScene);
}
