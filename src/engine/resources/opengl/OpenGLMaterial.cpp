#include "OpenGLMaterial.h"

using namespace Nyanners::Resources::OpenGL;

OpenGLMaterial::OpenGLMaterial() {
	// TODO: swap out shader for Shader::create()
	shader = new Shader();
	texture = Texture::create(TextureType::Texture2D);
	shader->load_from_file("assets/shaders/vertex.glsl", "assets/shaders/frag.glsl");

	shader->use();
	shader->setInt("uTexture", 0);
}

void OpenGLMaterial::use() {
	this->shader->use();
	glActiveTexture(GL_TEXTURE0);
	this->texture->use();
}

void OpenGLMaterial::release() {
	shader->release();
	texture->unuse();
}

void OpenGLMaterial::set_color(const DataTypes::Color3 &newColor) {
	shader->use();
	shader->setColor("uColor", newColor);
	shader->release();
}

void OpenGLMaterial::set_texture(Resources::Texture *newTexture) {
	texture->unuse();
	delete texture;
	texture = newTexture;
}

void OpenGLMaterial::set_texture(const std::filesystem::path &newTexturePath) {
	texture->load_from_file(newTexturePath);
}

void OpenGLMaterial::set_shader(Resources::Shader *newShader) {
	shader->release();
	delete shader;
	shader = newShader;
}

void OpenGLMaterial::set_shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath) {
	shader->load_from_file(vertexPath, fragmentPath);
}

