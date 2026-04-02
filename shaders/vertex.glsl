#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uModel;

uniform bool uEmmissive;

out vec3 vNormal;
out vec3 vWorldPos;

float noise(vec3 p) {
  return fract(sin(dot(p, vec3(12.9898,78.233,45.164))) * 43758.5453);
}

void main() {
  vec4 worldPos = uModel * vec4(aPos, 1.0);
  if (!uEmmissive) {
    float displacement = noise(aPos * 10.0) * 0.05;
    vec3 displacedPosition = aPos * (1.0 + displacement);
    worldPos = uModel * vec4(displacedPosition, 1.0);
  }

  vWorldPos = worldPos.xyz;

  gl_Position = uProjection * uView * worldPos;

  vNormal = mat3(transpose(inverse(uModel))) * aPos;
}
