#version 330 core
layout (location = 0) in vec3 position;

out vec3 vTexCoord;

uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    vTexCoord = position;

    mat4 view = mat4(mat3(uView)); // remove translation
    vec4 pos = uProjection * view * vec4(position, 1.0);

    gl_Position = pos.xyww; // force depth to 1.0
}