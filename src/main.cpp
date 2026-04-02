// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

#include "bodies.hpp"
#include "camera.hpp"
#include "settings.hpp"

void framebuffer_size_callback(GLFWwindow *, int width, int height);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
void processInput(GLFWwindow *window);

int scr_width = SCR_WIDTH, scr_height = SCR_HEIGHT;
float lastX = scr_width / 2.f, lastY = scr_height / 2.f;
bool firstMouse = true;
Camera camera(glm::vec3(-400.f, 300.f, 0.f), 0.f, -30.f);

float deltaTime = 0.f;
float lastFrame = 0.f;

int main() {
  if (!glfwInit())
    return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

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
  glEnable(GL_MULTISAMPLE);

  Bodies bodies;

  bodies.init();

  bodies.insert({0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, 10000, 50, {1.f, 0.9f, 0.6f},
                true);
  bodies.insert({600.f, 0.f, 0.f}, {0.f, 0.f, sqrt(G * 10000 / 600.f)}, 10, 12,
                {0.2f, 0.5f, 1.f}, false);
  bodies.insert({640.f, 0.f, 0.f},
                {0.f, 0.f, sqrt(G * 10000 / 600.f) + sqrt(G * 12.f / 40.f)},
                1.f, 4.f, {0.7f, 0.7f, 0.7f}, false);

  for (int i = 0; i < 100; ++i) {
    float r = glm::linearRand(200.f, 1500.f);
    float angle = glm::linearRand(0.f, 2.f * glm::pi<float>());
    float pitch = glm::linearRand(0.f, glm::pi<float>());

    glm::vec3 position = {r * cos(pitch) * sin(angle), r * sin(pitch), r * cos(pitch) * cos(angle)};

    float v = sqrt(G * 10000 / r);
    v *= glm::linearRand(0.9f, 1.1f);
    glm::vec3 velocity = {-sin(angle) * v, 0, cos(angle) * v};

    bodies.insert(position, velocity, 0.1f, 2.f, {0.5f, 0.5f, 0.5f}, false);
  }
  // Main loop
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    bodies.update(deltaTime);

    // Render
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(
        glm::radians(60.f), (float)scr_width / (float)scr_height, 0.001f,
        100000.f);
    glm::mat4 view = camera.getViewMatrix(RENDER_MULTIPLIER);

    bodies.render(projection, view, camera.position);

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
