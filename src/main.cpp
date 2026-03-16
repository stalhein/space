// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <glm/glm.hpp>

#include "camera.hpp"
#include "shader.hpp"

constexpr float G = 2000.f;
constexpr float THETA = 0.5f;
constexpr float EPS = 0.01f;

void framebuffer_size_callback(GLFWwindow *, int width, int height);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
void processInput(GLFWwindow *window);

constexpr unsigned int SCR_WIDTH = 800, SCR_HEIGHT = 600;
int scr_width = SCR_WIDTH, scr_height = SCR_HEIGHT;

float lastX = scr_width / 2.f, lastY = scr_height / 2.f;
bool firstMouse = true;
Camera camera(glm::vec3(-400.f, 300.f, 0.f), 0.f, -30.f);

float deltaTime = 0.f;
float lastFrame = 0.f;

struct Body {
  glm::vec3 position;
  glm::vec3 velocity;

  float mass;
  float radius;

  glm::vec3 colour;
  bool emmissive;
};

struct Node {
  glm::vec3 position;
  float size;

  glm::vec3 massCentre = {0, 0, 0};
  float mass = 0;

  Body *body = nullptr;
  Node *children[8] = {nullptr};
};

void addNodeVertices(Node *node, std::vector<float> *vertices);
void subdivide(Node *node);
int getQuad(Node *node, glm::vec3 position);
void insert(Node *node, Body *body);
void massNode(Node *node);
glm::vec3 getForce(Node *node, Body *body);
void deleteNode(Node *node);

int main() {
  if (!glfwInit())
    return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(scr_width, scr_height, "Space", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    glfwTerminate();
    return -1;
  }

  glEnable(GL_DEPTH_TEST);

  // Spheres
  Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");

  const int RESOLUTION = 30;
  std::vector<float> vertices;
  std::vector<unsigned int> elements;

  for (int lat = 0; lat <= RESOLUTION; ++lat) {
    const float alpha = lat * glm::pi<float>() / RESOLUTION;
    const float sinAlpha = sin(alpha);
    const float cosAlpha = cos(alpha);
    for (int lon = 0; lon <= RESOLUTION; ++lon) {
      const float beta = lon * 2 * glm::pi<float>() / RESOLUTION;
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

  for (int lat = 0; lat < RESOLUTION; ++lat) {
    for (int lon = 0; lon < RESOLUTION; ++lon) {
      const int one = (lat * (RESOLUTION + 1)) + lon;
      const int two = one + RESOLUTION + 1;

      elements.push_back(one);
      elements.push_back(two);
      elements.push_back(one + 1);

      elements.push_back(two);
      elements.push_back(two + 1);
      elements.push_back(one + 1);
    }
  }

  const int elementsCount = elements.size();

  GLuint vbo, vao, ebo;

  glCreateVertexArrays(1, &vao);
  glCreateBuffers(1, &vbo);
  glCreateBuffers(1, &ebo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned int),
               elements.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  glBindVertexArray(0);

  // Lines
  Shader lineShader("shaders/line_vertex.glsl", "shaders/line_fragment.glsl");

  std::vector<float> lines = {0.f, 0.f, 0.f, 100.f, 100.f, 100.f};

  GLuint lineVbo, lineVao;

  glCreateVertexArrays(1, &lineVao);
  glCreateBuffers(1, &lineVbo);

  glBindVertexArray(lineVao);
  glBindBuffer(GL_ARRAY_BUFFER, lineVbo);

  glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(float), lines.data(),
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  glBindVertexArray(0);

  // Tree
  Node root = {{-1000, -1000, -1000}, 2000};

  std::vector<Body> bodies;
  bodies.push_back({{400, 100, 0}, {0, 0, 120}, 500, 10, {0.f, 0.9f, 0.6f}, false});
  bodies.push_back({{0, 0, 0}, {0, 0, 0}, 10000, 30, {1.f, 0.8f, 0.3f}, true});

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    // Physics
    deleteNode(&root);
    for (auto &body : bodies) {
      insert(&root, &body);
    }

    massNode(&root);

    for (auto &body : bodies) {
      glm::vec3 acceleration = getForce(&root, &body);
      body.velocity += acceleration * deltaTime;
    }

    for (auto &body : bodies) {
      body.position += body.velocity * deltaTime;
    }

    // Render
    glClearColor(0.1f, 0.2f, 0.3f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection =
        glm::perspective(glm::radians(60.f),
                         (float)scr_width / (float)scr_height, 0.001f, 100000.f);
    glm::mat4 view = camera.getViewMatrix();

    shader.use();
    shader.setMat4("uProjection", projection);
    shader.setMat4("uView", view);

    glBindVertexArray(vao);
    for (auto body : bodies) {
      glm::mat4 model = glm::mat4(1.f);
      model = glm::translate(model, body.position);
      model = glm::scale(model, glm::vec3(body.radius * 2));
      shader.setMat4("uModel", model);

      shader.setVec3("uColour", body.colour);
      shader.setBool("uEmmissive", body.emmissive);

      glDrawElements(GL_TRIANGLES, elementsCount, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);

    // Render lines
    lines.clear();
    addNodeVertices(&root, &lines);

    glm::mat4 model = glm::mat4(1.f);
    lineShader.use();
    lineShader.setMat4("uProjection", projection);
    lineShader.setMat4("uView", view);
    lineShader.setMat4("uModel", model);

    glBindVertexArray(lineVao);
    glBindBuffer(GL_VERTEX_ARRAY, lineVbo);
    glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(float), lines.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_LINES, 0, lines.size() / 3);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

void framebuffer_size_callback(GLFWwindow *, int width, int height) {
  scr_width = width;
  scr_height = height;
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;

  lastX = xpos;
  lastY = ypos;

  camera.processMouse(xoffset, yoffset);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.processKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.processKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    camera.processKeyboard(UP, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    camera.processKeyboard(DOWN, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.processKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.processKeyboard(RIGHT, deltaTime);
}

void addNodeVertices(Node *node, std::vector<float> *vertices) {
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

void subdivide(Node *node) {
  node->body = nullptr;

  for (int i = 0; i < 8; ++i) {
    node->children[i] = new Node();
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

int getQuad(Node *node, glm::vec3 position) {
  glm::vec3 centre = node->position + glm::vec3(node->size/2.f);

  int index = 0;

  if (position.y >= centre.y)
    index += 4;
  if (position.x >= centre.x)
    index++;
  if (position.z >= centre.z)
    index += 2;

  return index;
}

void insert(Node *node, Body *body) {
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

void massNode(Node *node) {
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

glm::vec3 getForce(Node *node, Body *body) {
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

void deleteNode(Node *node) {
  if (node == nullptr)
    return;
  for (int i = 0; i < 8; ++i) {
    deleteNode(node->children[i]);
    delete node->children[i];
    node->children[i] = nullptr;
  }
}
