#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "body.hpp"
#include "body_octree.hpp"
#include "body_renderer.hpp"
#include "settings.hpp"
#include "shader.hpp"

class Bodies {
public:
  Bodies();

  void init();

  void insert(glm::vec3 position, glm::vec3 velocity, float mass, float radius,
               glm::vec3 colour, bool emmissive) {
    bodyOctree.addBody(position, velocity, mass, radius, colour, emmissive);
  }

  void update(float deltaTime);
  void render(glm::mat4 projection, glm::mat4 view, glm::vec3 playerPosition);

private:
  BodyRenderer renderer;
  BodyOctree bodyOctree;
};
