#pragma once
#include "glad/glad.h"
#include "core/Logger.h"

constexpr const char *glErrorToString(GLenum error) {
	switch (error) {
		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM";
		case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";
        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";
        case GL_STACK_UNDERFLOW:
            return "GL_STACK_UNDERFLOW";
        case GL_STACK_OVERFLOW:
            return "GL_STACK_OVERFLOW";
        default:
            return "UNKNOWN_GL_ERROR";
    }
}


#define GL_CHECK(x)                                             \
		do {                                                        \
		while (glGetError() != GL_NO_ERROR) {}                  \
		x;                                                      \
		GLenum err;                                             \
		while ((err = glGetError()) != GL_NO_ERROR) {           \
		Core::Logger::log(std::format(                                     \
		"[OpenGL Error] {} ({}:{})",                        \
		glErrorToString(err), __FILE__, __LINE__));      \
		__builtin_trap(); \
		}                                                       \
		} while (0)
