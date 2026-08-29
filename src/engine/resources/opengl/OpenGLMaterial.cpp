#include "OpenGLMaterial.h"

#include "instances/services/RenderingService.h"
#include "utils/glCheck.h"

using namespace Nyanners::Resources::OpenGL;

OpenGLMaterial::OpenGLMaterial() {
	// TODO: swap out shader for Shader::create()
	shader = Services::RenderingService::instance()->create_shader("assets/shaders/vertex.glsl", "assets/shaders/frag.glsl");
	texture = Texture::create(Texture2D);

	shader->use();
	shader->setInt("uTexture", 0);
	shader->setBool("uTextureSet", false);
}

void OpenGLMaterial::use() const {
	this->shader->use();
	GL_CHECK(glActiveTexture(GL_TEXTURE0));
	this->texture->use();
}

void OpenGLMaterial::release() const {
	shader->release();
	texture->unuse();
}

void OpenGLMaterial::set_color(const DataTypes::Color3 newColor) {
	shader->use();
	shader->setColor("uColor", newColor);
	color = newColor;
	shader->release();
}

void OpenGLMaterial::set_texture(const std::shared_ptr<Resources::Texture>& newTexture) {
	texture->unuse();
	texture = newTexture;
	texture->use();

	shader->use();
	shader->setBool("uTextureSet", true);
}

void OpenGLMaterial::set_texture(const std::filesystem::path &newTexturePath) {
	texture->use();
	texture->load_from_file(newTexturePath);
	shader->use();
	shader->setBool("uTextureSet", true);
}

void OpenGLMaterial::set_shader(Ref<Shader> newShader) {
	shader->release();
	newShader.reset();
	shader = newShader;
}

void OpenGLMaterial::set_shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath) {
	shader->load_from_file(vertexPath, fragmentPath);
}

