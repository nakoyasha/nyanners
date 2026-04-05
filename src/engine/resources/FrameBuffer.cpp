#include "FrameBuffer.h"
#include "core/Logger.h"
#include "third_party/sfml/src/SFML/Graphics/GLCheck.hpp"

using namespace Nyanners::Resources;

FrameBuffer::FrameBuffer(const int width, const int height) {
	framebufferTexture = new Resources::Texture();
	glGenFramebuffers(1, &framebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

	this->framebufferTexture->use();
	this->framebufferTexture->upload_buffer(GL_RGBA8, GL_RGBA, width, height, nullptr);
	glCheck(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->framebufferTexture->textureId, 0));

	glCheck(glGenRenderbuffers(1, &renderBufferId));
	glCheck(glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId));
	glCheck(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
	glCheck(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferId));

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
	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, this->framebufferId));
}

void FrameBuffer::release() {
	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

GLuint FrameBuffer::get_texture_id() {
	return this->framebufferTexture->textureId;
}

void FrameBuffer::resize(const int width, const int height) {
	this->use();
	framebufferTexture = new Resources::Texture();
	glGenFramebuffers(1, &framebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

	this->framebufferTexture->use();
	this->framebufferTexture->upload_buffer(GL_RGBA8, GL_RGBA, width, height, nullptr);
	glCheck(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->framebufferTexture->textureId, 0));

	glCheck(glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId));
	glCheck(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
	glCheck(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferId));

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