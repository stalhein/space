#include "body_octree.hpp"

const char* octreeVertex = R"(
#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

void main() {
  gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
}
)";

const char* octreeFragment = R"(
#version 330 core

out vec4 FragColor;

void main() {
  FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
)";

BodyOctree::BodyOctree(glm::vec3 position, float size)
    : root(nullptr), shader(octreeVertex, octreeFragment) {}

void BodyOctree::init() {
  glCreateVertexArrays(1, &vao);
  glCreateBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  glBindVertexArray(0);


  pool.resize(200000);
}

void BodyOctree::addBody(glm::vec3 position, glm::vec3 velocity, float mass,
                         float radius, glm::vec3 colour, bool emmissive) {
  Body body = {position, velocity, mass, radius, colour, emmissive};
  bodies.push_back(body);

  if (emmissive) {
    lights.push_back({{position.x, position.y, position.z, 1.f},
                      {colour.r, colour.g, colour.b, 1.f}});
  }
}

void BodyOctree::update(float deltaTime) {
  poolIdx = 0;
  root = nextNode();
  root->position = {-100000000.f, -100000000.f, -100000000.f};
  root->size = 200000000.f;

  for (auto &body : bodies) {
    insert(root, &body);
  }

  massNode(root);

  for (auto &body : bodies) {
    glm::vec3 acceleration = getForce(root, &body);
    body.velocity += acceleration * deltaTime;
  }

  for (auto &body : bodies) {
    body.position += body.velocity * deltaTime;
  }
}

void BodyOctree::render(glm::mat4 projection, glm::mat4 view) {
  vertices.clear();
  addNodeVertices(root, &vertices);

  glm::mat4 model = glm::mat4(1.f);
  model = translate(model, glm::vec3(RENDER_MULTIPLIER));
  model = scale(model, glm::vec3(RENDER_MULTIPLIER));
  shader.use();
  shader.setMat4("uProjection", projection);
  shader.setMat4("uView", view);
  shader.setMat4("uModel", model);

  glBindVertexArray(vao);
  glBindBuffer(GL_VERTEX_ARRAY, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_DYNAMIC_DRAW);
  glDrawArrays(GL_LINES, 0, vertices.size() / 3);
  glBindVertexArray(0);
}

void BodyOctree::addNodeVertices(Node *node, std::vector<float> *vertices) {
  if (node->children[0] == nullptr) {
    glm::vec3 p = node->position;
    float s = node->size;

    // clang-format off
    std::vector<float> newVerts = {
      // Back Face
      p.x,   p.y,   p.z,     p.x+s, p.y,   p.z,   
      p.x+s, p.y,   p.z,     p.x+s, p.y+s, p.z,   
      p.x+s, p.y+s, p.z,     p.x,   p.y+s, p.z,   
      p.x,   p.y+s, p.z,     p.x,   p.y,   p.z,
      
      // Front Face
      p.x,   p.y,   p.z+s,   p.x+s, p.y,   p.z+s, 
      p.x+s, p.y,   p.z+s,   p.x+s, p.y+s, p.z+s, 
      p.x+s, p.y+s, p.z+s,   p.x,   p.y+s, p.z+s, 
      p.x,   p.y+s, p.z+s,   p.x,   p.y,   p.z+s, 

      // Connecting Edges
      p.x,   p.y,   p.z,     p.x,   p.y,   p.z+s, 
      p.x+s, p.y,   p.z,     p.x+s, p.y,   p.z+s, 
      p.x+s, p.y+s, p.z,     p.x+s, p.y+s, p.z+s, 
      p.x,   p.y+s, p.z,     p.x,   p.y+s, p.z+s
    };
    // clang-format on
    vertices->insert(vertices->end(), newVerts.begin(), newVerts.end());

    return;
  }

  for (int i = 0; i < 8; ++i) {
    addNodeVertices(node->children[i], vertices);
  }
}

void BodyOctree::subdivide(Node *node) {
  node->body = nullptr;

  for (int i = 0; i < 8; ++i) {
    node->children[i] = nextNode();
    node->children[i]->size = node->size / 2.f;
  }

  float half = node->size / 2.f;
  glm::vec3 pos = node->position;

  node->children[0]->position = pos;
  node->children[1]->position = glm::vec3(pos.x + half, pos.y, pos.z);
  node->children[2]->position = glm::vec3(pos.x, pos.y, pos.z + half);
  node->children[3]->position = glm::vec3(pos.x + half, pos.y, pos.z + half);
  node->children[4]->position = glm::vec3(pos.x, pos.y + half, pos.z);
  node->children[5]->position = glm::vec3(pos.x + half, pos.y + half, pos.z);
  node->children[6]->position = glm::vec3(pos.x, pos.y + half, pos.z + half);
  node->children[7]->position =
      glm::vec3(pos.x + half, pos.y + half, pos.z + half);
}

int BodyOctree::getQuad(Node *node, glm::vec3 position) {
  glm::vec3 centre = node->position + glm::vec3(node->size / 2.f);

  int index = 0;

  if (position.y >= centre.y)
    index += 4;
  if (position.x >= centre.x)
    index++;
  if (position.z >= centre.z)
    index += 2;

  return index;
}

void BodyOctree::insert(Node *node, Body *body) {
  if (node->size < 1.f)
    return;

  if (node->body == nullptr && node->children[0] == nullptr) {
    node->body = body;
    return;
  }

  if (node->body != nullptr) {
    Body *tempBody = node->body;
    node->body = nullptr;

    subdivide(node);

    int quad = getQuad(node, tempBody->position);
    insert(node->children[quad], tempBody);
  }

  int quad = getQuad(node, body->position);
  insert(node->children[quad], body);
}

void BodyOctree::massNode(Node *node) {
  if (node->body != nullptr) {
    node->mass = node->body->mass;
    node->massCentre = node->body->position;
    return;
  }

  if (node->children[0] == nullptr)
    return;

  float mass = 0.f;
  glm::vec3 centre = {0, 0, 0};
  for (int i = 0; i < 8; ++i) {
    massNode(node->children[i]);
    mass += node->children[i]->mass;
    centre += node->children[i]->mass * node->children[i]->massCentre;
  }

  node->mass = mass;
  if (mass > 0)
    centre /= mass;
  node->massCentre = centre;
}

glm::vec3 BodyOctree::getForce(Node *node, Body *body) {
  if (node->mass == 0)
    return {0, 0, 0};
  if (node->body == body)
    return {0, 0, 0};

  glm::vec3 direction = node->massCentre - body->position;
  float distance = glm::length(direction);

  if (node->children[0] == nullptr) {
    float force = G * node->mass / (distance * distance + EPS);
    return force * glm::normalize(direction);
  }

  if (node->size / distance < THETA) {
    float force = G * node->mass / (distance * distance + EPS);
    return force * glm::normalize(direction);
  }

  glm::vec3 force = {0, 0, 0};

  if (node->children[0] != nullptr) {
    for (int i = 0; i < 8; ++i) {
      force += getForce(node->children[i], body);
    }
  }

  return force;
}

Node *BodyOctree::nextNode() {
  Node *node = &pool[poolIdx++];

  node->body = nullptr;
  node->mass = 0.f;
  node->massCentre = glm::vec3(0.f);
  for (int i = 0; i < 8; ++i) {
    node->children[i] = nullptr;
  }

  return node;
}
