#pragma once 

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <vector>
#include <cmath>

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
private:
    GLuint vbo, vao, ebo;

    int elementsCount = 0;

    int nextBodyID = 0;
    std::vector<Body> bodies;
};