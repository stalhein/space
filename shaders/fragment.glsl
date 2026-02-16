#version 330 core

out vec4 FragColor;

in vec3 vNormal;


void main() {
    vec3 lightDir = normalize(vec3(0.0, 1.0, 0.5));
    const vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 objectColour = vec3(0.2, 0.4, 0.6);

    const float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    vec3 normal = normalize(vNormal);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = vec3(ambient + diffuse) * objectColour;

    FragColor = vec4(result, 1.0);
}