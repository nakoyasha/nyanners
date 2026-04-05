#pragma once
#include <vector>
#include "resources/Mesh.h"

namespace Nyanners::Core {
	struct RenderCommand {
		Resources::Mesh* mesh;
		glm::mat4 transform;
	};

	struct RenderQueue {
		std::vector<RenderCommand> opaque;
		std::vector<RenderCommand> transparent;
	};
}