#version 300 es
precision highp float;

out vec4 FragColor;

in vec3 vFragPos;
in vec3 vNormal;

uniform vec3 uColour;
uniform float uEmmiter;

void main() {
    vec3 lightDir = normalize(-vFragPos);
    const vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 objectColour = uColour;

    const float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    vec3 normal = normalize(vNormal);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = objectColour;

    if (uEmmiter == 0.0) result = vec3(ambient + diffuse) * result;

    FragColor = vec4(result, 1.0);
}