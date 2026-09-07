#pragma once

namespace Nyanners::Shaders {
	inline constexpr auto FALLBACK_FRAGMENT = R"GLSL(
#version 330 core

layout(location = 0) out vec4 color;

void main() {
    color = vec4(1.0f, 0.0f, 1.0f, 1.0f);
}
)GLSL";

	inline constexpr auto FALLBACK_VERTEX = R"GLSL(
#version 330 core

layout(location = 0) in vec4 position;

uniform mat4 uView;
uniform mat4 uModel;
uniform mat4 uProjection;

void main() {
    gl_Position = (uProjection * uView * uModel) * position;
}
)GLSL";
}