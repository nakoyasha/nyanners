#version 330 core

out vec4 color;
in vec2 tex_coord;

uniform sampler2D r_texture;

void main(){
  // color = vec3(1,0,0);
  color = texture(r_texture, tex_coord);
}