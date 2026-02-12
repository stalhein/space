#version 300 es
precision highp float;

out vec4 FragColor;

in vec3 vNormal;

void main() {
    const vec3 lightDir = vec3(0.2, 0.5, 0.8);
    const vec3 lightColor = vec3(1.0, 1.0, 1.0);
    const vec3 objectColor = vec3(0.2, 0.5, 0.9);

    const float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    vec3 normal = normalize(vNormal);
    float diff = max(dot(normal, lightColor), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = vec3(ambient + diffuse) * objectColor;

    FragColor = vec4(result, 1.0);
}