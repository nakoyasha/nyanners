#include "FrameBuffer.h"
#include "core/Logger.h"


using namespace Nyanners::Resources;

FrameBuffer::FrameBuffer(const int width, const int height) {
	framebufferTexture = Resources::Texture::create(TextureType::Texture2D);
	glGenFramebuffers(1, &framebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

	this->framebufferTexture->use();
	this->framebufferTexture->upload_buffer(GL_RGB, GL_RGB, width, height, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, static_cast<GLuint>(reinterpret_cast<uintptr_t>(this->framebufferTexture->get_texture_handle())), 0);

	glGenRenderbuffers(1, &renderBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
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
	this->use();

	this->framebufferTexture->unuse();
	delete framebufferTexture;
	framebufferTexture = Texture::create(TextureType::Texture2D);
	framebufferTexture->use();

	framebufferTexture->upload_buffer(GL_RGB, GL_RGB, width, height, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, static_cast<GLuint>(reinterpret_cast<uintptr_t>(this->framebufferTexture->get_texture_handle())), 0);

	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferId);

	// auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	//
	// if (status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER) {
	// 	Core::Logger::log("[Resources::Framebuffer] Framebuffer could not be built because of an incomplete draw attachment");
	// } else if (status == GL_FRAMEBUFFER_UNSUPPORTED) {
	// 	Core::Logger::log("[Resources::Framebuffer] Framebuffer could not be created because the mode is unsupported");
	// } else if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
	// 	Core::Logger::log("[Resources::Framebuffer] Framebuffer could not be built because of an incomplete attachment");
	// } else if (status != GL_FRAMEBUFFER_COMPLETE) {
	// 	Core::Logger::log("[Resources::Framebuffer] Framebuffer failed to be completed... for some reason");
	// }

	this->framebufferTexture->unuse();
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	this->release();
}