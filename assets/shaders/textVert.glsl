#version 330 core

layout(location = 0) in vec4 position;

out vec2 vTexCoord;
uniform mat4 uProjection;
uniform mat4 uTransform;

void main() {
  gl_Position = (uProjection * uTransform) * vec4(position.xy, 0.0, 1.0);
  vTexCoord = position.zw;
}