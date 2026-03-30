// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

// Values that stay constant for the whole mesh.
uniform mat4 transform;

void main(){
  gl_Position =  transform * vec4(vertexPosition_modelspace,1.0f);
}