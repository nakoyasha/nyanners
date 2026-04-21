#pragma once
#include "Shader.h"
#include "Texture.h"

namespace Nyanners::Resources {
	class Material {
	public:
		Resources::Shader* shader {};
		std::shared_ptr<Resources::Texture> texture {};

		DataTypes::Color3* color = new DataTypes::Color3{255, 255, 255, 255};

		virtual void use() = 0;
		virtual void release() = 0;

		virtual void set_color(DataTypes::Color3* newColor) = 0;
		virtual void set_texture(std::shared_ptr<Resources::Texture> newTexture) = 0;
		virtual void set_texture(const std::filesystem::path& newTexturePath) = 0;

		virtual void set_shader(Resources::Shader* newShader) = 0;
		virtual void set_shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath) = 0;

		virtual ~Material() = default;

		static Material* create();
	};
}