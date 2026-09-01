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
			{"UI", static_cast<int>(ShaderStage::UI)},
			{"PostLighting", static_cast<int>(ShaderStage::PostLighting)},
			{"PostProcessing", static_cast<int>(ShaderStage::PostProcessing)},
		});

		Services::ReflectionService::create_descriptor("Shader", {"Object"})
			.add_constructor<Shader>()
			.add_property_chained<Shader, int, &Shader::get_priority, &Shader::set_priority>("Priority", Integer)
			.add_enum_property_chained<Shader, ShaderStage, &Shader::get_shader_stage, &Shader::set_shader_stage>("RunStage", "ShaderStage")
			.add_method<&Shader::compile>(
				"compile", Null, {{"forceCompile", Boolean}}
			)
			.add_method<&Shader::setFloat>("set_float", Null, {{"float", Number}})
			.add_method<&Shader::setBool>("set_bool", Null, {{"bool", Boolean}})
			.add_method<&Shader::setColor>("set_color", Null, {{"color", Color}})
			.add_method<&Shader::setVector2>("set_vector2", Null, {{"vector", Vector2}})
			.add_method<&Shader::setVector3>("set_vector3", Null, {{"vector", Vector3}})
			.add_method<&Shader::set_priority>("set_priority", Null, {{"priority", Number}});
	});
}

void Shader::load_from_file(
  const std::filesystem::path &newVertexPath,
  const std::filesystem::path &newFragmentPath
) {
	this->vertexPath = newVertexPath;
	this->fragmentPath = newFragmentPath;

	// !! shader requires re-compilation !!
	shaderCompiled = false;
}

void Shader::load_from_memory(const std::string &vertex, const std::string &fragment) {
	this->vertexSource = vertex;
	this->fragmentSource = fragment;

	shaderCompiled = false;
}

void Shader::setBool(const std::string &name, const bool value) {
	const bool used = use_if_not_used();

	glUniform1i(
	  glGetUniformLocation(shaderId, name.c_str()), static_cast<int>(value)
	);

	if (used) {
		this->release();
	}
}
void Shader::setInt(const std::string &name, const int value) {
	const bool used = use_if_not_used();

	glUniform1i(glGetUniformLocation(shaderId, name.c_str()), value);

	if (used) {
		this->release();
	}
}
void Shader::setFloat(const std::string &name, const float value) {
	const bool used = use_if_not_used();

	glUniform1f(glGetUniformLocation(shaderId, name.c_str()), value);

	if (used) {
		this->release();
	}
}

void Shader::setMatrix(const std::string &name, const glm::mat4 &value) {
	const bool used = use_if_not_used();

	GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(shaderId, name.c_str()), 1, GL_FALSE, glm::value_ptr(value)));

	if (used) {
		this->release();
	}
}

void Shader::setVector2(const std::string &name, const glm::vec2 &value) {
	const bool used = use_if_not_used();

	GL_CHECK(glUniform2f(glGetUniformLocation(shaderId, name.c_str()), value.x, value.y));

	if (used) {
		this->release();
	}
}

void Shader::setVector3(const std::string &name, const glm::vec3 &value) {
	const bool used = use_if_not_used();

	GL_CHECK(glUniform3f(glGetUniformLocation(shaderId, name.c_str()), value.x, value.y, value.z));

	if (used) {
		this->release();
	}
}

void Shader::setColor(
  const std::string &name, const DataTypes::Color3 value
) {
	const bool used = use_if_not_used();

	GL_CHECK(
		glUniform4f(
		glGetUniformLocation(shaderId, name.c_str()),
		  value.r / 255.0f,
		  value.g / 255.0f,
		  value.b / 255.0f,
		  value.alpha / 255.0f
	));

	if (used) {
		this->release();
	}
}

ShaderStage Shader::get_shader_stage() const {
	return stage;
}

void Shader::set_shader_stage(const ShaderStage& stage) {
	this->stage = stage;
}

int Shader::get_priority() const {
	return this->priority;
}

void Shader::set_priority(int newPriority) {
	const auto render = Services::RenderingService::instance();
	this->priority = newPriority;

	if (render->is_shader_post_process(std::dynamic_pointer_cast<Shader>(shared_from_this()))) {
		render->resort_post_processing_shaders_by_priority();
	}
}

bool Shader::use_if_not_used() {
	if (!this->shaderInUse) {
		this->use();
		return true;
	}

	return false;
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

	shaderInUse = true;
	GL_CHECK(glUseProgram(this->shaderId));
}

void Shader::release() {
	shaderInUse = false;
	GL_CHECK(glUseProgram(0));
}

void Shader::compile(const bool forceCompile) {
	if (shaderCompiled && !forceCompile) {
		return;
	}

	GLuint vertexShader = -99;
	GLuint fragmentShader = -99;

	if (this->sourceType == ShaderSourceType::File) {
		vertexShader = Services::RenderingService::compile_shader(GL_VERTEX_SHADER, this->vertexPath);
		fragmentShader = Services::RenderingService::compile_shader(GL_FRAGMENT_SHADER, this->fragmentPath);
	} else if (this->sourceType == ShaderSourceType::Memory) {
		vertexShader = Services::RenderingService::compile_shader(GL_VERTEX_SHADER, this->vertexSource);
		fragmentShader = Services::RenderingService::compile_shader(GL_FRAGMENT_SHADER, this->fragmentSource);
	}

	if (vertexShader <= 0 || fragmentShader <= 0) {
		throw std::runtime_error("Shader source invalid");
	}
	const auto programId = Services::RenderingService::compile_program(vertexShader, fragmentShader);

	if (programId == -1) {
		throw std::runtime_error("Shader compilation failed");
	}

	if (this->sourceType == ShaderSourceType::File) {
		Core::Logger::log(
		  std::format(
		    "Compiled and loaded {}, {} with id {}",
		    vertexPath.string(),
		    fragmentPath.string(),
		    programId
		  )
		);
	}

	shaderId = programId;
	shaderCompiled = true;
}