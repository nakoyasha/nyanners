#version 330 core

layout(location = 0) out vec4 color;

uniform sampler2D uSceneTexture;
in vec2 vUV;

void main() {
    color = texture(uSceneTexture, vUV);
}