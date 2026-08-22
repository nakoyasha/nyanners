#include "Shader.h"
#include "gtc/type_ptr.hpp"
#include "instances/services/RenderingService.h"
#include "utils/glCheck.h"

using namespace Nyanners::Resources;

namespace Nyanners::Scripting {
	static auto shaderDescriptor = ReflectionDescriptorRegistry::instance()->create_registrator([]() {
		Services::ReflectionService::register_enum("ShaderStage", {
			{"GeometryOpaque", static_cast<int>(ShaderStage::GeometryOpaque)},
			{"GeometryTransparent", static_cast<int>(ShaderStage::GeometryTransparent)},
			{"Lighting", static_cast<int>(ShaderStage::Lighting)},
			{"PostLighting", static_cast<int>(ShaderStage::PostLighting)},
			{"PostProcessing", static_cast<int>(ShaderStage::PostProcessing)},
		});

			Services::ReflectionService::create_descriptor("Shader", {"Object"})
			.add_constructor<Resources::Shader>()
			.add_enum_property_chained<Shader, ShaderStage, &Shader::get_shader_stage, &Shader::set_shader_stage>("RunStage", "ShaderStage")
			.add_method<&Resources::Shader::compile>(
				"compile", Null, {{"forceCompile", Boolean}}
			);
	});
}

void Shader::load_from_file(
  const std::filesystem::path &vertexPath,
  const std::filesystem::path &fragmentPath
) {
	this->vertexPath = vertexPath;
	this->fragmentPath = fragmentPath;

	// !! shader requires re-compilation !!
	shaderCompiled = false;
}

void Shader::setBool(const std::string &name, const bool value) const {
	glUniform1i(
	  glGetUniformLocation(shaderId, name.c_str()), static_cast<int>(value)
	);
}
void Shader::setInt(const std::string &name, const int value) const {
	glUniform1i(glGetUniformLocation(shaderId, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, const float value) const {
	glUniform1f(glGetUniformLocation(shaderId, name.c_str()), value);
}

void Shader::setMatrix(const std::string &name, const glm::mat4 &value) const {
	GL_CHECK(glUniformMatrix4fv(
	  glGetUniformLocation(shaderId, name.c_str()),
	  1,
	  GL_FALSE,
	  glm::value_ptr(value)
	));
}

void Shader::setColor(
  const std::string &name, const DataTypes::Color3 value
) const {
	GL_CHECK(glUniform4f(
	  glGetUniformLocation(shaderId, name.c_str()),
	  value.r / 255.0f,
	  value.g / 255.0f,
	  value.b / 255.0f,
	  value.alpha / 255.0f
	));
}

ShaderStage Shader::get_shader_stage() const {
	return stage;
}

void Shader::set_shader_stage(const ShaderStage& stage) {
	this->stage = stage;
}

Shader::~Shader() {
	glDeleteProgram(shaderId);
	shaderId = 0xDEADBEEF;
}

void Shader::use() {
	if (!shaderCompiled) {
		compile();
	}

	if (shaderId == 0xDEADBEEF || shaderId >= 50000) {
		throw std::runtime_error("Cannot use shader while unloaded");
	}

	GL_CHECK(glUseProgram(this->shaderId));
}

void Shader::release() const {
	GL_CHECK(glUseProgram(0));
}

void Shader::compile(const bool forceCompile) {
	if (shaderCompiled && !forceCompile) {
		return;
	}

	const auto vertexShader = Services::RenderingService::compile_shader(
	  GL_VERTEX_SHADER, this->vertexPath
	);
	const auto fragmentShader = Services::RenderingService::compile_shader(
	  GL_FRAGMENT_SHADER, this->fragmentPath
	);

	const auto programId =
	  Services::RenderingService::compile_program(vertexShader, fragmentShader);

	if (programId == -1) {
		throw std::runtime_error("Shader compilation failed");
	}

	Core::Logger::log(
	  std::format(
	    "Compiled and loaded {}, {} with id {}",
	    vertexPath.string(),
	    fragmentPath.string(),
	    programId
	  )
	);
	name = std::format("{}-{}", vertexPath.string(), fragmentPath.string());

	shaderId = programId;
	shaderCompiled = true;
}