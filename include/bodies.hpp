#pragma once 

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <vector>
#include <cmath>

#include "shader.hpp"

struct Body
{
    glm::dvec3 position;
    glm::dvec3 velocity;
    float radius;
    float mass;
};

class Bodies
{
public:
    Bodies();
    ~Bodies();

    void render(glm::mat4 projection, glm::mat4 view);
private:
    Shader shader;

    GLuint vbo, vao, ebo;

    int elementsCount = 0;

    int nextBodyID = 0;
    std::vector<Body> bodies;

    void addBody(glm::dvec3 position, glm::dvec3 velocity, float radius, float mass);
};