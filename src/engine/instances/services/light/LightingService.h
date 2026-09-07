#pragma once
#include "core/Service.h"
#include "Light.h"
#include "instances/Instance.h"

namespace Nyanners::Services {
	struct GPULight {
		glm::vec4 color;
		glm::vec3 position;
		float range;
	};

	struct HybridLight {
		const Instances::Light* cpu;
		GPULight gpu;
	};

	class LightingService : public Service<LightingService>, public Instances::Instance {
	public:
		LightingService() : Instance("LightingService") {}
		DataTypes::Color3 ambientLightColor;
		std::vector<HybridLight> lights;

		void add_light(const Instances::Light* light);
		void remove_light(const Instances::Light* light);
		HybridLight& get_light(const Instances::Light* light);

		DataTypes::Color3 get_ambient_color() const;
		void set_ambient_color(const DataTypes::Color3& color);
	private:
		GPULight make_gpu(const Instances::Light* source) const;
	};
}
