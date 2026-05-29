#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;

out vec2 vTexCoord;

uniform mat4 uTransform;
uniform mat4 uView;
uniform mat4 uProjection;

void main() {
  gl_Position = (uProjection * uTransform) * vec4(position.xy, 0.0, 1.0);
  vTexCoord = texCoord;
}