#include "FrameBuffer.h"
#include "core/Logger.h"


using namespace Nyanners::Resources;

FrameBuffer::FrameBuffer(const int width, const int height) {
	int normalizedWidth = width, normalizedHeight = height;

	if (width <= 0) {
		normalizedWidth = 1;
	}

	if (height <= 0) {
		normalizedHeight = 1;
	}

	framebufferTexture = Resources::Texture::create(TextureType::Texture2D);
	glGenFramebuffers(1, &framebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

	this->framebufferTexture->use();
	this->framebufferTexture->upload_buffer(GL_RGB, GL_RGB, normalizedWidth, normalizedHeight, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, static_cast<GLuint>(reinterpret_cast<uintptr_t>(this->framebufferTexture->get_texture_handle())), 0);

	glGenRenderbuffers(1, &renderBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, normalizedWidth, normalizedHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferId);

	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER) {
		Core::Logger::log("[Resources::Framebuffer] Framebuffer could not be built because of an incomplete draw attachment");
	} else if (status == GL_FRAMEBUFFER_UNSUPPORTED) {
		Core::Logger::log("[Resources::Framebuffer] Framebuffer could not be created because the mode is unsupported");
	} else if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
		Core::Logger::log("[Resources::Framebuffer] Framebuffer could not be built because of an incomplete attachment");
	} else if (status != GL_FRAMEBUFFER_COMPLETE) {
		Core::Logger::log("[Resources::Framebuffer] Framebuffer failed to be completed... for some reason");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	this->framebufferTexture->unuse();
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	size.x = width;
	size.y = height;
}

FrameBuffer::~FrameBuffer() {
	glDeleteFramebuffers(1, &framebufferId);
	glDeleteRenderbuffers(1, &renderBufferId);
}

void FrameBuffer::use() {
	glBindFramebuffer(GL_FRAMEBUFFER, this->framebufferId);
}

void FrameBuffer::release() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint FrameBuffer::get_texture_id() {
	return static_cast<GLuint>(reinterpret_cast<uintptr_t>(this->framebufferTexture->get_texture_handle()));
}

void FrameBuffer::resize(const int width, const int height) {
	int normalizedWidth = width, normalizedHeight = height;

	if (width <= 0) {
		normalizedWidth = 1;
	}

	if (height <= 0) {
		normalizedHeight = 1;
	}

	// don't recompute
	if (this->size.x == normalizedWidth && this->size.y == normalizedHeight) {
		return;
	}

	Core::Logger::log(std::format("Resizing framebuffer to {},{}", normalizedWidth, normalizedHeight));

	this->use();
	this->framebufferTexture->unuse();
	this->framebufferTexture.reset();

	framebufferTexture = Texture::create(TextureType::Texture2D);
	framebufferTexture->use();

	framebufferTexture->upload_buffer(GL_RGB, GL_RGB, normalizedWidth, normalizedHeight, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, static_cast<GLuint>(reinterpret_cast<uintptr_t>(this->framebufferTexture->get_texture_handle())), 0);

	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, normalizedWidth, normalizedHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferId);

	this->framebufferTexture->unuse();
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	this->release();
	size.x = width;
	size.y = height;
}