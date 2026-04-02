#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "body.hpp"
#include "settings.hpp"
#include "shader.hpp"

struct LightData {
  Light lights[100];
  int numberLights;
  float padding[3];
};

class BodyRenderer {
public:
  BodyRenderer();
  void init();

  void render(glm::mat4 projection, glm::mat4 view, glm::vec3 playerPosition,
              std::vector<Body> &bodies, std::vector<Light> &lights);

private:
  GLuint vbo, vao, ebo, lightUbo;

  std::vector<float> vertices;
  std::vector<unsigned int> indices;

  Shader shader;
};
