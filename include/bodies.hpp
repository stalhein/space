#pragma once 

#include <glad/glad.h>

#include <vector>

class Bodies
{
public:
    Bodies();
    ~Bodies();
private:
    GLuint vbo, vao, ebo;

    int elementsCount = 0;

    int nextBodyID = 0;
    std::vector<int> bodies;
};