//
#include "OpenGLTexture.h"
#include "utils/glCheck.h"

using namespace Nyanners::Resources::OpenGL;

OpenGLTexture::OpenGLTexture(const TextureType type) {
	if (type == TextureType::Texture2D) {
		textureType = GL_TEXTURE_2D;
	} else if (type == TextureType::Cubemap) {
		textureType = GL_TEXTURE_CUBE_MAP;
	} else {
		throw std::runtime_error("Failed creating texture: Unsupported TextureType");
	}

	GL_CHECK(glGenTextures(1, &textureId));
	GL_CHECK(glBindTexture(textureType, textureId));

	GL_CHECK(glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL_CHECK(glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CHECK(glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GL_CHECK(glBindTexture(textureType, 0));
}

OpenGLTexture::OpenGLTexture(const TextureType type, const std::filesystem::path &path) : OpenGLTexture(type) {
	OpenGLTexture::load_from_file(path);
}

OpenGLTexture::~OpenGLTexture() {
	GL_CHECK(glDeleteTextures(1, &textureId));
}

void OpenGLTexture::load_from_file(const std::filesystem::path &path) {
	this->use();
	this->load_file_into_buffer(path);
	this->upload_buffer(
	  GL_RGBA8,
	  GL_RGBA,
	  static_cast<int>(width),
	  static_cast<int>(height),
	  textureBuffer
	);
	this->unuse();
}

void OpenGLTexture::upload_buffer(const int internalFormat, const int externalFormat, const int width, const int height, void* imageBuffer
) {
	this->textureBuffer = imageBuffer;
	this->width = width;
	this->height = height;

	GL_CHECK(glTexImage2D(textureType, 0, internalFormat, width, height, 0, externalFormat, GL_UNSIGNED_BYTE, imageBuffer));
}

void OpenGLTexture::set_mipmap_enabled(const bool newState) {
	if (newState == true) {
		GL_CHECK(glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		GL_CHECK(glGenerateMipmap(textureType));
	} else {
		GL_CHECK(glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	}

	useMipmaps = newState;
}

void OpenGLTexture::use() {
	glBindTexture(textureType, textureId);
}

void OpenGLTexture::unuse() {
	glBindTexture(textureType, 0);
}

void *OpenGLTexture::get_texture_handle() const {
	return reinterpret_cast<uintptr_t*>(static_cast<uintptr_t>(this->textureId));
}