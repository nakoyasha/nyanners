#version 330 core

layout(location = 0) out vec4 color;

in vec2 vTexCoord;

uniform vec4 uColor;
uniform sampler2D uTexture;
uniform bool uTextureSet;
uniform bool uFlipTexture;

void main() {
    vec2 texCoord = vTexCoord;

    if (uFlipTexture) {
        texCoord = vec2(texCoord.x, 1.0 - vTexCoord.y);
    }

    if (uTextureSet) {
        vec4 texColor = texture(uTexture, texCoord);
        color = uColor * texColor;
    } else {
        color = uColor;
    }
    // color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}