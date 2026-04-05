#pragma once
#include "instances/drawable/Drawable.h"
#include "RenderQueue.h"

namespace Nyanners::Core {
	class Renderer {
	public:
		virtual ~Renderer() = default;

		virtual void initialize() = 0;
		virtual void begin_frame() = 0;
		virtual void render(const Instances::Drawable& drawable);
		virtual void end_frame() = 0;
		virtual void shutdown() = 0;
	};
}