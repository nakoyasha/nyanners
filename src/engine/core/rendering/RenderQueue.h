#pragma once
#include <vector>

#include "resources/Material.h"
#include "resources/Mesh.h"

namespace Nyanners::Core {
	struct RenderCommand {
		const Resources::Mesh* mesh;
		const Resources::Material* material;
		Resources::FrameBuffer* framebuffer;
	};

	struct RenderQueue {
		std::vector<RenderCommand> opaque;
		std::vector<RenderCommand> transparent;
	};
}