#pragma once

#include <glm/glm.hpp>

struct Body {
  glm::vec3 position;
  glm::vec3 velocity;

  float mass;
  float radius;

  glm::vec3 colour;
  bool emmissive;
};

struct Light {
  glm::vec4 position;
  glm::vec4 colour;
};


