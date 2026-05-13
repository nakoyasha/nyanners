#version 330 core
in vec3 vTexCoord;
out vec4 color;

uniform vec4 uColor;
uniform samplerCube skybox;

void main()
{
    color = uColor * texture(skybox, vTexCoord);
}