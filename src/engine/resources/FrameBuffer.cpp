#include "FrameBuffer.h"
#include "core/Logger.h"
#include "utils/glCheck.h"

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
	framebufferTexture->debugIdentifier = "FBTexture";

	GL_CHECK(glGenFramebuffers(1, &framebufferId));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, framebufferId));

	this->framebufferTexture->use();
	this->framebufferTexture->upload_buffer(
	  GL_RGB, GL_RGB, normalizedWidth, normalizedHeight, nullptr
	);
	GL_CHECK(glFramebufferTexture2D(
	  GL_FRAMEBUFFER,
	  GL_COLOR_ATTACHMENT0,
	  GL_TEXTURE_2D,
	  static_cast<GLuint>(reinterpret_cast<uintptr_t>(
	    this->framebufferTexture->get_texture_handle()
	  )),
	  0
	));

	GL_CHECK(glGenRenderbuffers(1, &renderBufferId));
	GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId));
	GL_CHECK(glRenderbufferStorage(
	  GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, normalizedWidth, normalizedHeight
	));
	GL_CHECK(glFramebufferRenderbuffer(
	  GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferId
	));
	GL_CHECK(glViewport(0, 0, normalizedWidth, normalizedHeight));

	check_status();

	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	this->framebufferTexture->unuse();
	GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, 0));

	size.x = width;
	size.y = height;
}

FrameBuffer::~FrameBuffer() {
	GL_CHECK(glDeleteFramebuffers(1, &framebufferId));
	GL_CHECK(glDeleteRenderbuffers(1, &renderBufferId));
}

void FrameBuffer::use() const {
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, this->framebufferId));
}

void FrameBuffer::release() const {
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void FrameBuffer::clear() {
	this->use();
	GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	this->release();
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

	if (normalizedWidth == 1 && normalizedHeight == 1) {
		// Core::Logger::log_error("FrameBuffer::resize called with very small newSize");
		// __builtin_trap();
	}

	// don't recompute
	if (this->size.x == normalizedWidth && this->size.y == normalizedHeight) {
		return;
	}

	Core::Logger::log(std::format("Resizing framebuffer to {},{}", normalizedWidth, normalizedHeight));

	this->use();
	framebufferTexture->use();
	framebufferTexture->upload_buffer(GL_RGB, GL_RGB, normalizedWidth, normalizedHeight, nullptr);

	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, static_cast<GLuint>(reinterpret_cast<uintptr_t>(this->framebufferTexture->get_texture_handle())), 0));

	GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId));
	GL_CHECK(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, normalizedWidth, normalizedHeight));
	GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferId));

	check_status();
	GL_CHECK(glViewport(0, 0, normalizedWidth, normalizedHeight));

	this->framebufferTexture->unuse();
	GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, 0));
	this->release();

	size.x = normalizedWidth;
	size.y = normalizedHeight;
}

void FrameBuffer::construct_framebuffer(const int width, const int height) {
	int normalizedWidth = width, normalizedHeight = height;

	if (width <= 0) {
		normalizedWidth = 1;
	}

	if (height <= 0) {
		normalizedHeight = 1;
	}

	framebufferTexture = Resources::Texture::create(TextureType::Texture2D);
	GL_CHECK(glGenFramebuffers(1, &framebufferId));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, framebufferId));

	this->framebufferTexture->use();
	this->framebufferTexture->upload_buffer(GL_RGB, GL_RGB, normalizedWidth, normalizedHeight, nullptr);
	GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, static_cast<GLuint>(reinterpret_cast<uintptr_t>(this->framebufferTexture->get_texture_handle())), 0));

	GL_CHECK(glGenRenderbuffers(1, &renderBufferId));
	GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId));
	GL_CHECK(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, normalizedWidth, normalizedHeight));
	GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferId));

	check_status();

	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	this->framebufferTexture->unuse();
	GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, 0));

	size.x = width;
	size.y = height;
}

void FrameBuffer::check_status() {
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
}