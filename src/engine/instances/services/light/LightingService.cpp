#include "LightingService.h"
#include <algorithm>

#include "instances/services/EngineService.h"
#include "instances/services/ReflectionService.h"
#include "scripting/reflections/ReflectionDescriptorRegistry.h"

using namespace Nyanners::Services;

namespace Nyanners::Scripting {
	[[maybe_unused]]
	static auto lightingService = ReflectionDescriptorRegistry::instance()->create_registrator([]() {
		ReflectionService::create_descriptor("LightingService", {"Instance"})
			.add_property_chained<LightingService, DataTypes::Color3, &LightingService::get_ambient_color, &LightingService::set_ambient_color>("AmbientColor", Reflection::ReflectionPropertyType::Color);
	});
}



void LightingService::add_light(const Light* light) {
	const auto hybrid = HybridLight {
		.cpu = light,
		.gpu = make_gpu(light)
	};

	lights.push_back(hybrid);
}

HybridLight& LightingService::get_light(const Light* light) {
	const auto hybrid = std::ranges::find_if(lights, [light](const HybridLight& hybrid) {
		return hybrid.cpu == light;
	});

	if (hybrid == lights.end()) {
		EngineService::panic("Attempt to retrieve light that wasn't made into a HybridLight");
	}

	return *hybrid;
}

Nyanners::DataTypes::Color3 LightingService::get_ambient_color() const {
	return this->ambientLightColor;
}

void LightingService::set_ambient_color(const DataTypes::Color3 &color) {
	this->ambientLightColor = color;
}

void LightingService::remove_light(const Light* light) {
	for (auto hybrid = lights.begin(); hybrid != lights.end(); ++hybrid) {
		if (hybrid->cpu == light) {
			lights.erase(hybrid);
			return;
		}
	}
}

GPULight LightingService::make_gpu(const Light *source) const {
	const auto light = GPULight {
		.color = glm::vec4 {source->color.r, source->color.g, source->color.b, source->color.alpha},
		.position = source->position,
		.range = 20
	};

	return light;
}
