#version 330 core

struct Light {
  vec4 position;
  vec4 colour;
};

layout(std140) uniform uLightBlock {
  Light lights[100];
  int numberLights;
};

uniform vec3 uColour;
uniform bool uEmmissive;
uniform vec3 uPlayerPosition;

in vec3 vWorldPos;
in vec3 vNormal;

out vec4 FragColor;

void main() {
  vec3 normal = normalize(vNormal);
  vec3 viewDirection = normalize(uPlayerPosition - vWorldPos);

  if (uEmmissive) {
    FragColor = vec4(uColour, 1.0);
    return;
  }


  vec3 result = vec3(0.0001) * uColour;

  for (int i = 0; i < numberLights; ++i) {
    vec3 lightPosition = lights[i].position.xyz;
    vec3 direction = lightPosition - vWorldPos;
    float distance = length(direction);
    vec3 lightDir = normalize(direction);

    float diffuseStrength = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * lights[i].colour.rgb;

    float attenuation = min(1.0 / (distance * distance), 1.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDirection, reflectDir), 0.0), 128.0);
    vec3 specular = spec * lights[i].colour.rgb * 0.01;
    
    result += (diffuse + specular) * uColour * attenuation;
  }

  float rim = 1.0 - max(dot(viewDirection, normal), 0.0);
  rim = pow(rim, 3.0);
  vec3 rimLight = rim * uColour * 0.01;
  result += rimLight;

  float noise = fract(sin(dot(vWorldPos.xyz, vec3(12.9898,78.233,45.164))) * 43758.5453);
  result *= 0.9 + noise * 0.2;

  result = pow(result, vec3(1.0 / 2.2));

  FragColor = vec4(result, 1.0);
}

