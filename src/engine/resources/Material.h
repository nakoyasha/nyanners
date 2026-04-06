#pragma once
#include "Shader.h"
#include "Texture.h"

namespace Nyanners::Resources {
	class Material {
	public:
		Resources::Shader* shader {};
		Resources::Texture* texture {};

		DataTypes::Color3 color {255, 255, 255, 255};

		virtual void use() = 0;
		virtual void release() = 0;

		virtual void set_color(const DataTypes::Color3& newColor) = 0;
		virtual void set_texture(Resources::Texture* newTexture) = 0;
		virtual void set_texture(const std::filesystem::path& newTexturePath) = 0;

		virtual void set_shader(Resources::Shader* newShader) = 0;
		virtual void set_shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath) = 0;

		virtual ~Material() = default;

		static Material* create();
	};
}