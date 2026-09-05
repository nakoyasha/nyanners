#version 330 core

layout(location = 0) in vec4 uPosition;
layout(location = 1) in vec2 uTexCoord;
layout(location = 2) in vec3 uNormal;

uniform mat4 uView;
uniform mat4 uModel;
uniform mat4 uProjection;

out vec2 vTexCoord;
out vec3 vFragPos;
out vec3 vNormal;

void main() {
    vFragPos = vec3(uModel * uPosition);
    vNormal = mat3(transpose(inverse(uModel))) * uNormal;
    vTexCoord = uTexCoord;
    vNormal = uNormal;

    gl_Position = (uProjection * uView * uModel) * uPosition;
}