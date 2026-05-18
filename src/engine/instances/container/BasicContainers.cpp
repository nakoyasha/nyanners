#include "BasicContainers.h"
#include "instances/services/ReflectionService.h"

namespace Nyanners::Instances {
	void link_basic_containers() {
		// TODO: combine meshes in a Model? I guess
		Services::ReflectionService::create_descriptor("Model", {"Instance"})
		.add_constructor<Model>();

		Services::ReflectionService::create_descriptor("Folder", {"Instance"})
		.add_constructor<Folder>();
	}
}

