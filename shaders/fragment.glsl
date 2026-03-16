#version 330 core

uniform vec3 uColour;
uniform bool uEmmissive;

in vec3 vWorldPos;
in vec3 vNormal;

out vec4 FragColor;

void main() {
  vec3 lightDir = normalize(-vWorldPos);
  const vec3 lightColour = vec3(1.0);

  vec3 result = uColour;

  if (uEmmissive) {
    FragColor = vec4(result, 1.0);
    return;
  }

  const float ambientStrength = 0.2;
  vec3 ambient = ambientStrength * lightColour;

  vec3 normal = normalize(vNormal);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = diff * lightColour;

  result *= vec3(ambient + diffuse);

  FragColor = vec4(result, 1.0);
}

