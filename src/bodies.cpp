#include "bodies.hpp"

constexpr float SPHERE_RESOLUTION = 30;

Bodies::Bodies() : bodyOctree({-100000000.f, -100000000.f, -100000000.f}, 200000000.f) {}

void Bodies::init() {
  renderer.init();
  bodyOctree.init();
}

void Bodies::update(float deltaTime) {
  bodyOctree.update(deltaTime);
}

void Bodies::render(glm::mat4 projection, glm::mat4 view,
                    glm::vec3 playerPosition) {

  renderer.render(projection, view, playerPosition, bodyOctree.bodies, bodyOctree.lights);
}
