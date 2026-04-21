#pragma once
#include "resources/Material.h"

namespace Nyanners::Resources::OpenGL {
	class OpenGLMaterial : public Material {
	public:
		OpenGLMaterial();

		void use() override;
		void release() override;

		void set_color(DataTypes::Color3 *newColor) override;
		void set_texture(std::shared_ptr<Resources::Texture> newTexture) override;
		void set_texture(const std::filesystem::path& newTexturePath) override;

		void set_shader(Resources::Shader* newShader) override;
		void set_shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath) override;
	};
}