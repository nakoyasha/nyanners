#include "Material.h"

#include "instances/services/ReflectionService.h"
#include "opengl/OpenGLMaterial.h"
#include "scripting/reflections/ReflectionDescriptorRegistry.h"

namespace Nyanners::Scripting {
	[[maybe_unused]]
	static auto materialDescriptor = ReflectionDescriptorRegistry::instance()->create_registrator([]() {
		Services::ReflectionService::create_descriptor("Material", {"Object"})
		.add_property_chained<Resources::Material, Ref<Resources::Shader>, &Resources::Material::get_shader>("Shader", ReflectionPropertyType::Instance)
		.add_method<&Resources::Material::set_shader_path>("set_shader", Null, {});
	});
}

using namespace Nyanners::Resources;

void Material::set_shader_path(const std::filesystem::path &vertexPath, const std::filesystem::path &fragmentPath) {
	this->set_shader(vertexPath, fragmentPath);
}

Ref<Shader> Material::get_shader() const {
	return this->shader;
}

Ref<Material> Material::create() {
	return std::make_shared<OpenGL::OpenGLMaterial>();
}
