#pragma once
#include "render/model.h"
#include "render/input/gamepad.h"

namespace Render
{
    struct ParticleEmitter;
}

namespace Game
{

struct Tile {
    glm::vec3 position; //center of tile
    glm::mat4 transform;
    glm::mat4 rotation;
    int id;
    float rotationY;
    glm::vec3 edge; //Edge where tile ends

    Tile() {};
    Tile(glm::vec3& position, glm::mat4& transform, glm::mat4& rotation, glm::vec3 edge, int id) {
        this->position = position;
        this->transform = transform;
        this->rotation = rotation;
        this->id = id;
        this->edge = edge;
        rotationY = 0;
    }
};

struct Podracer
{
    Podracer();

    glm::vec3 racerPos = glm::vec3(0.0f, 0.f, 4.0f);
    glm::mat4 transform = glm::mat4(1);
    glm::vec3 linearVelocity = glm::vec3(0);

    const float normalSpeed = 1.0f;
    const float boostSpeed = normalSpeed * 2.0f;

    bool disableControls = true;
    bool disableCollisions = false;
    bool automatic = false;
    float movementIndex = 4.f;
    float currentSpeed = 0.0f;
    float currentSideSpeed = 0.0f;
    float upcomingRotationY = 0;

    const float* gamepadAxis;
    const unsigned char* gamepadButtons;

    float rotationY = 0;
    float positionX = 0;
    float cameraX = 0;
    float cameraY = 0;

    Render::ModelId model;
    //Render::ParticleEmitter* particleEmitterLeft;
    //Render::ParticleEmitter* particleEmitterRight;
    //float emitterOffset = -0.5f;

    int Update(float dt, std::vector<Tile>& tiles);
    void reset();

    bool CheckCollisions();
    
    const glm::vec3 colliderEndPoints[9] = {
        glm::vec3(-0.50657, -0.480347, -0.346542),  // right wing
        glm::vec3(0.50657, -0.480347, -0.346542),  // left wing
        glm::vec3(-0.342382, 0.25109, -0.010299),   // right top
        glm::vec3(0.342382, 0.25109, -0.010299),   // left top
        glm::vec3(-0.285614, -0.10917, 0.869609), // right front
        glm::vec3(0.285614, -0.10917, 0.869609), // left front
        glm::vec3(0.0, -0.10917, 0.569609), // middle front
        glm::vec3(-0.279064, -0.10917, -0.98846),   // right back
        glm::vec3(0.279064, -0.10917, -0.98846)   // right back
    };
};

}