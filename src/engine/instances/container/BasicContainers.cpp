#include "BasicContainers.h"
#include "instances/services/ReflectionService.h"

namespace Nyanners::Instances {
	void link_basic_containers() {
			const auto& model = Nyanners::Services::ReflectionService::create_reflection({
					.className = "Model",
					.base = "Instance",
					.flags = {Nyanners::Scripting::Reflection::Creatable},
					.constructor = &Nyanners::Scripting::Reflection::create_instance<Nyanners::Instances::Model>,
					.properties = {},
					.methods = {}
			});
			const auto& folder = Nyanners::Services::ReflectionService::create_reflection({
				.className = "Folder",
				.base = "Instance",
				.flags = {Nyanners::Scripting::Reflection::Creatable},
				.constructor = &Nyanners::Scripting::Reflection::create_instance<Nyanners::Instances::Folder>,
				.properties = {},
				.methods = {}
		});
	}
}

