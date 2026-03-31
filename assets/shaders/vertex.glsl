// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout (location = 1) in vec3 aPos;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;

// Values that stay constant for the whole mesh.
uniform mat4 transform;
out vec3 color;
out vec2 tex_coord;

void main() {
  gl_Position =  transform * vec4(vertexPosition_modelspace,1.0f);
  color = aColor;
  tex_coord = aTexCoord;
}