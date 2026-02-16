#pragma once 

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <array>
#include <cmath>
#include <algorithm>

#include "shader.hpp"

#include "constants.hpp"

struct Body
{
    glm::dvec3 position;
    glm::dvec3 velocity;
    float radius;
    float mass;
    glm::vec3 colour;
};

class Bodies
{
public:
    Bodies();

    void update(float deltaTime);
    void render(glm::mat4 projection, glm::mat4 view);
private:
    Shader shader;

    GLuint vbo, vao, ebo;

    int elementsCount = 0;

    std::vector<Body> bodies;
    int noBodies = 0;

    void addBody(glm::dvec3 position, glm::dvec3 velocity, float radius, float mass, glm::vec3 colour);

    std::vector<glm::dvec3> getAccelerations();
};