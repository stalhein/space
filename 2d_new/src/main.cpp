#include <SDL3/SDL.h>
#include <glm/glm.hpp>

#include <vector>

constexpr int SCR_SIZE = 600;

constexpr float G = 1000.f;
constexpr float THETA = 0.5f;
constexpr float EPS = 0.01f;

struct Body {
  glm::vec2 position;
  glm::vec2 velocity;
  float mass;
  float radius;
  glm::vec3 colour;
};

struct Node {
  glm::vec2 position;
  float size;

  float mass = 0;
  glm::vec2 centreOfMass = {0, 0};

  Body *body = nullptr;

  Node *children[4] = {nullptr};
};

void renderNode(SDL_Renderer *renderer, Node *node);
void subdivide(Node *node);
int getQuad(Node *node, glm::vec2 position);
void insert(Node *node, Body *body);
void massTree(Node *root);
glm::vec2 getForce(Node *node, Body *body);
void deleteTree(Node *node);

int main() {
  if (!SDL_Init(SDL_INIT_VIDEO))
    return -1;

  SDL_Window *window = SDL_CreateWindow("Space", SCR_SIZE, SCR_SIZE, 0);
  if (!window)
    return -1;

  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
  if (!renderer)
    return -1;

  Node root = {{0, 0}, SCR_SIZE};

  std::vector<Body> bodies;
  bodies.push_back({{100, 300}, {0, 10}, 10, 10, {100, 100, 200}});
  bodies.push_back({{400, 300}, {0, 0}, 200, 30, {200, 100, 100}});

  bool running = true;
  SDL_Event event;
  Uint64 lastTime = SDL_GetTicks();
  while (running) {
    Uint64 time = SDL_GetTicks();
    float dt = (time - lastTime) / 1000.f;
    lastTime = time;
    if (dt > 0.05f)
      dt = 0.05f;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT)
        running = false;
    }

    // Build quad-tree
    deleteTree(&root);

    for (auto &body : bodies) {
      insert(&root, &body);
    }

    massTree(&root);

    // Gravity
    for (auto &body : bodies) {
      glm::vec2 acceleration = getForce(&root, &body);

      body.velocity += acceleration * dt;
    }

    for (auto &body : bodies) {
      body.position += body.velocity * dt;
    }

    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render bodies
    for (auto &body : bodies) {
      SDL_SetRenderDrawColor(renderer, body.colour.r, body.colour.g,
                             body.colour.b, 255);
      SDL_FRect shape = {body.position.x - body.radius / 2,
                         body.position.y - body.radius / 2, body.radius,
                         body.radius};
      SDL_RenderFillRect(renderer, &shape);
    }

    // Render quad-tree
    SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
    renderNode(renderer, &root);

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

void renderNode(SDL_Renderer *renderer, Node *node) {
  if (node->children[0] == nullptr) {
    SDL_FRect shape = {node->position.x, node->position.y, node->size,
                       node->size};
    SDL_RenderRect(renderer, &shape);
    return;
  }
  SDL_RenderPoint(renderer, node->centreOfMass.x, node->centreOfMass.y);

  for (int i = 0; i < 4; ++i) {
    renderNode(renderer, node->children[i]);
  }
}

void subdivide(Node *node) {
  node->body = nullptr;

  for (int i = 0; i < 4; ++i) {
    node->children[i] = new Node();
    node->children[i]->size = node->size / 2;
  }

  float halfSize = node->size / 2;

  node->children[0]->position = node->position;
  node->children[1]->position = node->position + glm::vec2(halfSize, 0);
  node->children[2]->position = node->position + glm::vec2(0, halfSize);
  node->children[3]->position = node->position + glm::vec2(halfSize);
}

int getQuad(Node *node, glm::vec2 position) {
  float cX = node->position.x + node->size / 2;
  float cY = node->position.y + node->size / 2;

  int index = 0;

  if (position.x >= cX)
    index++;
  if (position.y >= cY)
    index += 2;

  return index;
}

void insert(Node *node, Body *body) {
  if (node->body == nullptr && node->children[0] == nullptr) {
    node->body = body;
    return;
  }

  if (node->body != nullptr) {
    Body *oldBody = node->body;
    node->body = nullptr;

    subdivide(node);

    int quad = getQuad(node, oldBody->position);
    insert(node->children[quad], oldBody);
  }

  int quad = getQuad(node, body->position);
  insert(node->children[quad], body);
}

void massTree(Node *node) {
  if (node->body != nullptr) {
    node->mass = node->body->mass;
    node->centreOfMass = node->body->position;
    return;
  }
  if (node->children[0] == nullptr) {
    return;
  }

  float mass = 0;
  glm::vec2 centre = {0, 0};
  for (int i = 0; i < 4; ++i) {
    massTree(node->children[i]);
    mass += node->children[i]->mass;
    centre += node->children[i]->mass * node->children[i]->centreOfMass;
  }
  node->mass = mass;
  if (mass > 0)
    centre /= mass;
  node->centreOfMass = centre;
}

glm::vec2 getForce(Node *node, Body *body) {
  if (node->mass == 0)
    return {0, 0};
  if (node->body == body)
    return {0, 0};

  glm::vec2 direction = node->centreOfMass - body->position;
  float distance = glm::length(direction);

  if (node->children[0] == nullptr) {
    float force = G * node->mass / (distance * distance + EPS);
    return force * glm::normalize(direction);
  }

  if (node->size / distance < THETA) {
    float force = G * node->mass / (distance * distance + EPS);
    return force * glm::normalize(direction);
  }

  glm::vec2 force{0, 0};

  for (int i = 0; i < 4; ++i) {
    if (node->children[0] != nullptr) {
      force += getForce(node->children[i], body);
    }
  }

  return force;
}

void deleteTree(Node *node) {
  if (node == nullptr)
    return;
  for (int i = 0; i < 4; ++i) {
    if (node->children[i] != nullptr) {
      deleteTree(node->children[i]);
      delete node->children[i];
      node->children[i] = nullptr;
    }
  }
}
