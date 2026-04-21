#version 330 core

layout(location = 0) out vec4 color;

in vec2 vTexCoord;

uniform vec4 uColor;
uniform sampler2D uTexture;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(uTexture, vTexCoord).r);
    color = uColor * sampled;
}