#pragma once
#include "glad/glad.h"
#include "instances/Instance.h"
#include "instances/datatypes/Color3.h"
#include <glm/gtc/matrix_transform.hpp>
#include <filesystem>


namespace Nyanners::Resources {
	enum class ShaderStage {
		GeometryOpaque = 0,
		GeometryTransparent = 1,
		Lighting = 2,
		PostLighting = 3,
		PostProcessing = 4,
	};

	class Shader : public Instances::Instance {
	public:
		Shader() : Instance("Shader") {}
		~Shader();

		ShaderStage stage;

		void use();
		void release() const;
		void compile(bool forceCompile = false);
		void load_from_file(
		  const std::filesystem::path &vertexPath,
		  const std::filesystem::path &fragmentPath
		);

		void setBool(const std::string &name, const bool value) const;
		void setInt(const std::string &name, const int value) const;
		void setFloat(const std::string &name, const float value) const;
		void setMatrix(const std::string &name, const glm::mat4 &value) const;
		void setColor(
		  const std::string &name, const Nyanners::DataTypes::Color3 value
		) const;

		ShaderStage get_shader_stage() const;
		void set_shader_stage(const ShaderStage& stage);

	private:
		GLuint shaderId = 0xDEADBEEF;
		std::filesystem::path vertexPath;
		std::filesystem::path fragmentPath;

		bool shaderCompiled = false;
	};
}
