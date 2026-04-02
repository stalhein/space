#include "body_renderer.hpp"

constexpr int SPHERE_RESOLUTION = 30;

const char* sphereVertex = R"(
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
)";

const char* sphereFragment = R"(
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

)";

BodyRenderer::BodyRenderer()
    : shader(sphereVertex, sphereFragment) {}

void BodyRenderer::init() {

  // Spheres
  for (int lat = 0; lat <= SPHERE_RESOLUTION; ++lat) {
    const float alpha = lat * glm::pi<float>() / SPHERE_RESOLUTION;
    const float sinAlpha = sin(alpha);
    const float cosAlpha = cos(alpha);
    for (int lon = 0; lon <= SPHERE_RESOLUTION; ++lon) {
      const float beta = lon * 2 * glm::pi<float>() / SPHERE_RESOLUTION;
      const float sinBeta = sin(beta);
      const float cosBeta = cos(beta);

      const float x = sinAlpha * cosBeta;
      const float y = cosAlpha;
      const float z = sinAlpha * sinBeta;

      vertices.push_back(x);
      vertices.push_back(y);
      vertices.push_back(z);
    }
  }
  for (int lat = 0; lat < SPHERE_RESOLUTION; ++lat) {
    for (int lon = 0; lon < SPHERE_RESOLUTION; ++lon) {
      const int one = (lat * (SPHERE_RESOLUTION + 1)) + lon;
      const int two = one + SPHERE_RESOLUTION + 1;

      indices.push_back(one);
      indices.push_back(two);
      indices.push_back(one + 1);

      indices.push_back(two);
      indices.push_back(two + 1);
      indices.push_back(one + 1);
    }
  }

  glCreateVertexArrays(1, &vao);
  glCreateBuffers(1, &vbo);
  glCreateBuffers(1, &ebo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  glBindVertexArray(0);

  // Lighting buffer
  glGenBuffers(1, &lightUbo);
  glBindBuffer(GL_UNIFORM_BUFFER, lightUbo);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(LightData), NULL, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightUbo);
}

void BodyRenderer::render(glm::mat4 projection, glm::mat4 view,
                          glm::vec3 playerPosition, std::vector<Body> &bodies,
                          std::vector<Light> &lights) {

  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  shader.use();
  shader.setMat4("uProjection", projection);
  shader.setMat4("uView", view);
  shader.setVec3("uPlayerPosition", playerPosition);

  // Lighting
  LightData lightData;
  lightData.numberLights = lights.size();
  for (int i = 0; i < lightData.numberLights; ++i) {
    lightData.lights[i] = lights[i];
  }
  glBindBuffer(GL_UNIFORM_BUFFER, lightUbo);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightData), &lightData);

  // Render bodies
  glBindVertexArray(vao);
  for (auto body : bodies) {
    glm::mat4 model = glm::mat4(1.f);
    model = glm::translate(model, body.position * RENDER_MULTIPLIER);
    model = glm::scale(model, glm::vec3(body.radius * 2) * RENDER_MULTIPLIER);
    shader.setMat4("uModel", model);

    shader.setVec3("uColour", body.colour);
    shader.setBool("uEmmissive", body.emmissive);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  }
  glBindVertexArray(0);
}
