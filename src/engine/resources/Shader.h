#pragma once
#include "glad/glad.h"
#include "instances/Instance.h"
#include "instances/datatypes/Color3.h"
#include <glm/gtc/matrix_transform.hpp>
#include <filesystem>

#include "shaders/fallback.h"

namespace Nyanners::Resources {
	enum class ShaderStage {
		GeometryOpaque = 0,
		GeometryTransparent = 1,
		Lighting = 2,
		UI = 3,
		PostLighting = 4,
		PostProcessing = 5,
	};

	enum class ShaderSourceType {
		Memory = 0,
		File = 1,
	};

	class Shader : public Instances::Instance {
	public:
		Shader() : Instance("Shader") {}
		~Shader();

		ShaderStage stage = ShaderStage::GeometryOpaque;
		ShaderSourceType sourceType = ShaderSourceType::File;

		std::filesystem::path vertexPath;
		std::filesystem::path fragmentPath;

		std::string vertexSource = Shaders::FALLBACK_VERTEX;
		std::string fragmentSource = Shaders::FALLBACK_FRAGMENT;

		int priority = -1;

		virtual void use();
		virtual void release();
		void compile(bool forceCompile = false);

		void load_from_file(
		  const std::filesystem::path &newVertexPath,
		  const std::filesystem::path &newFragmentPath
		);

		void load_from_memory(
			const std::string &vertex,
			const std::string &fragment
		);

		void setBool(const std::string &name, const bool value);
		void setInt(const std::string &name, const int value);
		void setFloat(const std::string &name, const float value);
		void setMatrix(const std::string &name, const glm::mat4 &value);
		void setVector2(const std::string &name, const glm::vec2 &value);
		void setVector3(const std::string &name, const glm::vec3 &value);
		void setColor(const std::string &name, DataTypes::Color3 value);

		ShaderStage get_shader_stage() const;
		void set_shader_stage(const ShaderStage& stage);

		int get_priority() const;
		void set_priority(int newPriority);
	private:
		GLuint shaderId = 0xDEADBEEF;

		bool shaderInUse = false;
		bool shaderCompiled = false;
		bool shaderValid = false;


		// checks if it's not used and uses the shader if isn't, also returns true if the shader wasn't already used.
		bool use_if_not_used();
	};
}
