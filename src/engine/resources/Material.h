#pragma once
#include "Shader.h"
#include "Texture.h"

namespace Nyanners::Resources {
	class Material {
	public:
		Shader* shader {};
		std::shared_ptr<Texture> texture {};
		DataTypes::Color3 color = {255, 255, 255, 255};

		virtual void use() const = 0;
		virtual void release() const = 0;

		virtual void set_color(DataTypes::Color3 newColor) = 0;
		virtual void set_texture(const std::shared_ptr<Texture> &newTexture) = 0;
		virtual void set_texture(const std::filesystem::path& newTexturePath) = 0;

		virtual void set_shader(Shader* newShader) = 0;
		virtual void set_shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath) = 0;

		virtual ~Material() = default;

		static Material* create();
	private:
		bool shaderCompiled = false;
	};
}