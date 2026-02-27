#include "bodies.hpp"

constexpr double G = 6.67430e-11;


Bodies::Bodies() : shader("shaders/vertex.glsl", "shaders/fragment.glsl")
{
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

    // Sun
    addBody({0, 0, 0}, {0, 0, 0}, 696340000, 1.989e30, {1.0f, 0.9f, 0.2f});
    // Earth
    addBody({149.6e9, 0, 0}, {0, 0, 29780}, 6371000.0f, 5.972e24, {0.4f, 0.6f, 0.9f});
    // Moon
    addBody({149.6e9 + 384400000 * std::cos(0.08970992), 384400000 * std::sin(0.08970992), 0}, {0.0f, 0.0f, 29780.0f + 1022.0f}, 1737000, 7.35e22, {1.0f, 1.0f, 0.9f});
}

void Bodies::addBody(glm::dvec3 position, glm::dvec3 velocity, float radius, float mass, glm::vec3 colour)
{
    Body body;
    body.position = position;
    body.velocity = velocity;
    body.radius = radius;
    body.mass = mass;
    body.colour = colour;
    bodies.push_back(body);

    noBodies++;
}

std::vector<glm::dvec3> Bodies::getAccelerations()
{
    std::vector<glm::dvec3> accelerations(noBodies, glm::dvec3(0.0));
    
    for (int i = 0; i < bodies.size(); ++i) {
        for (int j = 0; j < bodies.size(); ++j) {
            if (i == j) continue;

            const auto& a = bodies[i];
            const auto& b = bodies[j];

            glm::dvec3 difference = b.position - a.position;
            double distanceSquared = glm::dot(difference, difference);

            double distance = std::max(distanceSquared, 1000.0);

            const double magnitude = (G * b.mass) / (distance * distance);


            accelerations[i] += (difference/distance) * magnitude;
        }
    }

    return accelerations;
}

void Bodies::update(float deltaTime)
{
    double dt = static_cast<double>(deltaTime * TIME_SCALE);

    auto a1 = getAccelerations();

    for (int i = 0; i < noBodies; ++i) {
        auto& body = bodies[i];

        body.position += (body.velocity * dt) + (a1[i] * 0.5 * dt * dt);
    }

    auto a2 = getAccelerations();

    for (int i = 0; i < noBodies; ++i) {
        auto& body = bodies[i];

        body.velocity += (a1[i] + a2[i]) * 0.5 * dt;
    }
}

void Bodies::render(glm::mat4 projection, glm::mat4 view)
{
    shader.use();
    shader.setMat4("uProjection", projection);
    shader.setMat4("uView", view);
    for (auto &body : bodies) {
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(body.position * RENDER_SCALE));
        model = glm::scale(model, glm::vec3(body.radius * (float)(2 * RENDER_SCALE)));
        shader.setMat4("uModel", model);

        shader.setVec3("uColour", body.colour);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, elementsCount, GL_UNSIGNED_INT, 0);
    }
}