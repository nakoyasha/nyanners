#include "OpenGLMaterial.h"

#include "instances/services/RenderingService.h"
#include "utils/glCheck.h"

using namespace Nyanners::Resources::OpenGL;

OpenGLMaterial::OpenGLMaterial() {
	// TODO: swap out shader for Shader::create()
	shader = Services::RenderingService::instance()->create_shader("assets/shaders/vertex.glsl","assets/shaders/frag.glsl");
	texture = Texture::create(Texture2D);

	shader->use();
	shader->setInt("uTexture", 0);
	shader->setBool("uTextureSet", false);
	shader->release();
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

void OpenGLMaterial::set_texture(const Ref<Texture>& newTexture) {
	texture->unuse();
	texture = std::move(newTexture);
	texture->use();

	shader->use();
	shader->setBool("uTextureSet", true);
	shader->release();
	texture->unuse();
}

void OpenGLMaterial::set_texture(const std::filesystem::path &newTexturePath) {
	texture->use();
	texture->load_from_file(newTexturePath);
	shader->use();
	shader->setBool("uTextureSet", true);
	shader->release();
}