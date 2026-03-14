#include <SDL3/SDL.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <cmath>
#include <array>

constexpr int SCR_SIZE = 600;

struct Body
{
    glm::vec2 position;
    float mass;
    float radius;
    glm::vec3 colour;
};

struct Node
{
    glm::vec2 position;
    float size;

    float mass = 0;
    glm::vec2 centreOfMass = {0, 0};

    Body* body = nullptr;

    Node* children[4] = {nullptr};
};

void renderNode(SDL_Renderer* renderer, Node* node);
void subdivide(Node* node);
int getQuad(Node* node, glm::vec2 position);
void insert(Node* node, Body* body);

int main()
{    
    if (!SDL_Init(SDL_INIT_VIDEO))
        return -1;

    SDL_Window* window = SDL_CreateWindow("Space", SCR_SIZE, SCR_SIZE, 0);
    if (!window)
        return -1;

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer)
        return -1;


    Node root = {{0, 0}, SCR_SIZE};

    std::vector<Body> bodies;
    bodies.push_back({{68, 49}, 10, 10, {100, 100, 200}});
    bodies.push_back({{198, 127}, 20, 15, {200, 100, 100}});
    bodies.push_back({{137, 194}, 30, 20, {100, 200, 100}});

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
            
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    
                }
            }
        }

        root.body = nullptr;
        for (int i = 0; i < 4; ++i) {
            root.children[i] = nullptr;
        }
        
        for (auto& body : bodies) {
            insert(&root, &body);
        }


        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (auto& body : bodies) {
            SDL_SetRenderDrawColor(renderer, body.colour.r, body.colour.g, body.colour.b, 255);
            SDL_FRect shape = {body.position.x-body.radius/2, body.position.y-body.radius/2, body.radius, body.radius};
            SDL_RenderFillRect(renderer, &shape);
        }

        SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
        renderNode(renderer, &root);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void renderNode(SDL_Renderer* renderer, Node* node)
{
    if (node->children[0] == nullptr) {
        SDL_FRect shape = {node->position.x, node->position.y, node->size, node->size};
        SDL_RenderRect(renderer, &shape);
        return;
    }

    for (int i = 0; i < 4; ++i) {
        renderNode(renderer, node->children[i]);
    }
}

void subdivide(Node* node)
{
    node->body = nullptr;

    for (int i = 0; i < 4; ++i) {
        node->children[i] = new Node();
        node->children[i]->size = node->size/2;
    }

    float halfSize = node->size/2;

    node->children[0]->position = node->position;
    node->children[1]->position = node->position + glm::vec2(halfSize, 0);
    node->children[2]->position = node->position + glm::vec2(0, halfSize);
    node->children[3]->position = node->position + glm::vec2(halfSize);
}

int getQuad(Node* node, glm::vec2 position)
{
    float cX = node->position.x + node->size/2;
    float cY = node->position.y + node->size/2;

    int index = 0;

    if (position.x >= cX) index++;
    if (position.y >= cY) index += 2;

    return index;
}

void insert(Node* node, Body* body)
{
    if (node->body == nullptr && node->children[0] == nullptr) {
        node->body = body;
        return;
    }

    if (node->body != nullptr) {
        Body* oldBody = node->body;
        node->body = nullptr;

        subdivide(node);

        int quad = getQuad(node, oldBody->position);
        insert(node->children[quad], oldBody);
    }

    int quad = getQuad(node, body->position);
    insert(node->children[quad], body);
}