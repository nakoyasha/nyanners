#pragma once
#include "Shader.h"
#include "Texture.h"

namespace Nyanners::Resources {
	class Material : public Instances::Object {
	public:
		Ref<Shader> shader {};
		Ref<Texture> texture {};
		DataTypes::Color3 color = {255, 255, 255, 255};

		virtual void use() const = 0;
		virtual void release() const = 0;

		virtual void set_color(DataTypes::Color3 newColor) = 0;
		virtual void set_texture(const Ref<Texture> &newTexture) = 0;
		virtual void set_texture(const std::filesystem::path& newTexturePath) = 0;

		virtual void set_shader(Ref<Shader> newShader) = 0;
		virtual void set_shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath) = 0;

		// for lua-specifics, TODO: figure out better way
		void set_shader_path(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
		Ref<Shader> get_shader() const;

		Material() : Object("Material") {};
		virtual ~Material() = default;

		static Ref<Material> create();
	private:
		bool shaderCompiled = false;
	};
}