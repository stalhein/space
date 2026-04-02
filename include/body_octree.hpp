#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>

#include "body.hpp"
#include "settings.hpp"
#include "shader.hpp"

struct Node {
  glm::vec3 position;
  float size;

  glm::vec3 massCentre = {0, 0, 0};
  float mass = 0;

  Body *body = nullptr;
  Node *children[8] = {nullptr};
};

class BodyOctree {
public:
  std::vector<Body> bodies;
  std::vector<Light> lights;

  Node *root;

  BodyOctree(glm::vec3 position, float size);

  void init();

  void addBody(glm::vec3 position, glm::vec3 velocity, float mass, float radius,
               glm::vec3 colour, bool emmissive);

  void update(float deltaTime);

  void render(glm::mat4 projection, glm::mat4 view);

private:
  std::vector<Node> pool;
  unsigned int poolIdx = 0;

  Shader shader;

  GLuint vbo, vao;

  std::vector<float> vertices;

  void addNodeVertices(Node *node, std::vector<float> *vertices);
  void subdivide(Node *node);
  int getQuad(Node *node, glm::vec3 position);
  void insert(Node *node, Body *body);
  void massNode(Node *node);
  glm::vec3 getForce(Node *node, Body *body);
  void deleteNode(Node *node);
  Node* nextNode();
};
