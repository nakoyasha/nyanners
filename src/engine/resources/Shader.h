#pragma once
#include "glad/glad.h"
#include "instances/basic/Object.h"
#include "instances/datatypes/Color3.h"
#include <glm/gtc/matrix_transform.hpp>
#include <filesystem>

namespace Nyanners::Resources {
	class Shader : public Nyanners::Instances::Object {
	public:
		Shader() : Object("Shader") {}
		~Shader();

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

	private:
		GLuint shaderId = 0xDEADBEEF;
		std::filesystem::path vertexPath;
		std::filesystem::path fragmentPath;

		bool shaderCompiled = false;
	};
}
