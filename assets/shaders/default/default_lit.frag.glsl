#version 330 core

layout(location = 0) out vec4 vColor;

in vec2 vTexCoord;
in vec3 vFragPos;
in vec3 vNormal;

uniform vec3 uLightPos;
uniform vec3 uViewPos;
uniform vec4 uLightColor;
uniform vec4 uAmbientColor;
uniform float uRange;
uniform vec4 uColor;

void main() {
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(uLightColor) * vec3(uAmbientColor);

    float distance = length(uLightPos - vFragPos);
    float attenuation = 1.0 - smoothstep(0.0, uRange, distance);

    attenuation *= attenuation;

    // diffuse
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(uLightPos - vFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(uLightColor) * attenuation;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(uViewPos - vFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
    vec3 specular = specularStrength * spec * vec3(uLightColor) * attenuation;

    vec3 result = (ambient + diffuse + specular) * vec3(uColor);
    vColor = vec4(result, 1.0);
}
