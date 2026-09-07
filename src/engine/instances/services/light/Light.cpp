#pragma once
#include "Light.h"

#include "LightingService.h"
#include "instances/services/ReflectionService.h"
#include "instances/services/RenderingService.h"
#include "scripting/reflections/ReflectionDescriptorRegistry.h"

namespace Nyanners::Scripting {
	[[maybe_unused]]
	static auto lightRegistrator = ReflectionDescriptorRegistry::instance()->create_registrator([]() {
		Services::ReflectionService::create_descriptor("Light", {"Instance"})
		.add_property_chained<Light, DataTypes::Color3, &Light::get_color, &Light::set_color>("Light", Color)
		.add_property_chained<Light, glm::vec3, &Light::get_position, &Light::set_position>("Position", Vector3)
		.add_property_chained<Light, double, &Light::get_range, &Light::set_range>("Range", Number)
		.add_constructor<Light>();
	});
}

Light::Light(): Instance("Light") {
	debug_IconMaterial = Resources::Material::create();
	debug_IconMaterial->set_color({255, 255, 255, 255});
	debug_IconMaterial->set_texture("assets/textures/editor/light.png");
	Services::LightingService::instance()->add_light(this);
}

Light::~Light() {
	Services::LightingService::instance()->remove_light(this);
}

Nyanners::DataTypes::Color3 Light::get_color() const {
	return color;
}

void Light::set_color(const DataTypes::Color3 newColor) {
	auto& hybrid = Services::LightingService::instance()->get_light(this);
	hybrid.gpu.color = {newColor.r, newColor.g, newColor.b, newColor.alpha};
	color = newColor;
}

glm::vec3 Light::get_position() const {
	return position;
}

void Light::set_position(const glm::vec3 newPosition) {
	auto& hybrid = Services::LightingService::instance()->get_light(this);
	hybrid.gpu.position = newPosition;
	this->position = newPosition;
}

void Light::draw() {
	const auto render = Services::RenderingService::instance();
	const auto camera = render->renderer->camera;

	glm::mat4 transform = glm::translate(glm::mat4(1.0f),position);
	const glm::mat4 cameraRotation = glm::mat4(glm::mat3(camera->view));
	transform *= glm::transpose(cameraRotation);
	transform = glm::scale(transform,glm::vec3(0.5f));

	Services::RenderingService::instance()->renderer->render_quad(debug_IconMaterial, transform);
}

double Light::get_range() const {
	return this->range;
}

void Light::set_range(const double range) {
	auto&[cpu, gpu] = Services::LightingService::instance()->get_light(this);
	gpu.range = range;
	this->range = range;
}
