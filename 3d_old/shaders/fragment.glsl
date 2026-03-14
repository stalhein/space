#version 330 core

out vec4 FragColor;

in vec3 vNormal;

in vec3 vWorldPos;

uniform vec3 uColour;
uniform bool uEmmissive;

void main() {
    vec3 lightDir = normalize(-vWorldPos);
    const vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 objectColour = uColour;

    vec3 result = objectColour;

    if (uEmmissive) {
        FragColor = vec4(result, 1.0);
        return;
    }
    
    const float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    vec3 normal = normalize(vNormal);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    result *= vec3(ambient + diffuse);

    FragColor = vec4(result, 1.0);
}