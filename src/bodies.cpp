#include "bodies.hpp"

Bodies::Bodies() : shader("../shaders/vertex.glsl", "../shaders/fragment.glsl")
{
    const int RESOLUTION = 30;

    std::vector<float> vertices;
    std::vector<int> elements;

    for (int lat = 0; lat <= RESOLUTION; ++lat) {
        const float alpha = lat * M_PI / RESOLUTION;
        const float sinAlpha = sin(alpha);
        const float cosAlpha = cos(alpha);
        for (int lon = 0; lon <= RESOLUTION; ++lon) {
            const float beta = lon * 2 * M_PI / RESOLUTION;
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
            elements.push_back(one+1);

            elements.push_back(two);
            elements.push_back(two+1);
            elements.push_back(one+1);
        }
    }

    elementsCount = elements.size();

    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vbo);
    glCreateBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned int), elements.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    addBody({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 10.0f, 10.0f);
}

Bodies::~Bodies()
{

}

void Bodies::addBody(glm::dvec3 position, glm::dvec3 velocity, float radius, float mass)
{
    Body body;
    body.position = position;
    body.velocity = velocity;
    body.radius = radius;
    body.mass = mass;
    bodies.push_back(body);
}

void Bodies::render(glm::mat4 projection, glm::mat4 view)
{
    shader.use();
    shader.setMat4("uProjection", projection);
    shader.setMat4("uView", view);

    glm::mat4 model(1.0f);
    shader.setMat4("uModel", model);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, elementsCount, GL_UNSIGNED_INT, 0);
}