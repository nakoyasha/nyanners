#version 330 core

in vec2 vUV;
layout(location = 0) out vec4 color;

uniform sampler2D uSceneTexture;
uniform vec2 uResolution;

#define CURVATURE 5.1
#define BLUR 0.012
#define CA_AMT 1.012

// this was ported from https://www.shadertoy.com/view/DlfSz8

void main() {
    // Curvature
    vec2 crtUV = vUV * 2.0 - 1.0;

    vec2 offset = crtUV.yx / CURVATURE;
    crtUV += crtUV * offset * offset;
    crtUV = crtUV * 0.5 + 0.5;

    // Screen edges
    vec2 edge =
        smoothstep(0.0, BLUR, crtUV) *
        (1.0 - smoothstep(1.0 - BLUR, 1.0, crtUV));

    // Chromatic aberration
    vec3 scene = vec3(
        texture(uSceneTexture, (crtUV - 0.5) * CA_AMT + 0.5).r,
        texture(uSceneTexture, crtUV).g,
        texture(uSceneTexture, (crtUV - 0.5) / CA_AMT + 0.5).b
    );

    scene *= edge.x * edge.y;

    // CRT scanlines
    float scanline = sin(vUV.y * uResolution.y * 3.14159);

    // Convert sine into dark horizontal lines
    float scan = 0.12 * abs(scanline);

    scene *= 1.0 - scan;

    color = vec4(scene, 1.0);
}