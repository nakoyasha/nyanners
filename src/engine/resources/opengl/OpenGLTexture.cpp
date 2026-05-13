//
#include "OpenGLTexture.h"
#include "utils/glCheck.h"
#include <cmath>

using namespace Nyanners::Resources::OpenGL;

OpenGLTexture::OpenGLTexture(const TextureType type, std::source_location location) {
	if (type == Texture2D) {
		textureType = GL_TEXTURE_2D;
	} else if (type == Cubemap) {
		textureType = GL_TEXTURE_CUBE_MAP;
	} else {
		throw std::runtime_error("Failed creating texture: Unsupported TextureType");
	}

	GL_CHECK(glGenTextures(1, &textureId));
	OpenGLTexture::use();

	OpenGLTexture::set_texture_parameter(MagnificationFilter, Linear);
	OpenGLTexture::set_texture_parameter(MinificationFilter, Linear);
	OpenGLTexture::set_texture_parameter(TextureWrapCoordinateS, ClampToEdge);
	OpenGLTexture::set_texture_parameter(TextureWrapCoordinateT, ClampToEdge);

	OpenGLTexture::unuse();

	const std::filesystem::path full_path(location.file_name());
	debugIdentifier = std::format("{}", full_path.filename().string());
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
	debugIdentifier = path.string();

	this->upload_buffer(
	  GL_RGBA8,
	  GL_RGBA,
	  width,
	  height,
	  textureBuffer
	);
	this->unuse();
}

void OpenGLTexture::upload_buffer(const int internalFormat, const int externalFormat, const int width, const int height, void* imageBuffer
) {
	this->textureBuffer = imageBuffer;
	this->width = width;
	this->height = height;

	GL_CHECK(glTexImage2D(textureType, 0, internalFormat, this->width, this->height, 0, externalFormat, GL_UNSIGNED_BYTE, imageBuffer));
}

void OpenGLTexture::upload_buffer_cubemap(
  CubemapSide side,
  int internalFormat,
  int externalFormat,
  int width,
  int height,
  void *imageBuffer
) {
	this->width += width;
	this->height = height;

	glTexImage2D(static_cast<GLenum>(side), 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageBuffer);
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

int getOpenGLWrapMode(const TextureWrapMode& mode) {
	switch (mode) {
		case Linear:
			return GL_LINEAR;
		case Nearest:
			return GL_NEAREST;
		case ClampToEdge:
			return GL_CLAMP_TO_EDGE;
		default:
			return GL_LINEAR;
	}
}

int getOpenGLFilterParameter(const TextureFilterParameter& parameter) {
	switch (parameter) {
		case TextureWrapCoordinateT:
			return GL_TEXTURE_WRAP_T;
		case TextureWrapCoordinateS:
			return GL_TEXTURE_WRAP_S;
		case TextureWrapCoordinateR:
			return GL_TEXTURE_WRAP_R;
		case MinificationFilter:
			return GL_TEXTURE_MIN_FILTER;
		case MagnificationFilter:
			return GL_TEXTURE_MAG_FILTER;
		default:
			throw std::invalid_argument("Cannot find a compatible OpenGL filter parameter");
	}
}

void OpenGLTexture::set_texture_parameter(
  const TextureFilterParameter &parameter, const TextureWrapMode &wrapMode
) {
	auto glParameter = getOpenGLFilterParameter(parameter);
	auto glWrap = getOpenGLWrapMode(wrapMode);

	GL_CHECK(glTexParameteri(textureType, glParameter, glWrap));
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