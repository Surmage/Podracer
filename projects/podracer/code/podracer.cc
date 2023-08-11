#include "config.h"
#include "podracer.h"
#include "render/input/inputserver.h"
#include "render/cameramanager.h"
#include "render/physics.h"
#include "render/debugrender.h"
#include "render/particlesystem.h"
#include <iostream>

using namespace Input;
using namespace glm;
using namespace Render;

namespace Game
{
Podracer::Podracer()
{
    uint32_t numParticles = 2048;
    this->particleEmitterLeft = new ParticleEmitter(numParticles);
    this->particleEmitterLeft->data = {
        .origin = glm::vec4(this->racerPos + (vec3(this->transform[2]) * emitterOffset),1),
        .dir = glm::vec4(glm::vec3(-this->transform[2]), 0),
        .startColor = glm::vec4(0.38f, 0.76f, 0.95f, 1.0f) * 2.0f,
        .endColor = glm::vec4(0,0,0,1.0f),
        .numParticles = numParticles,
        .theta = glm::radians(0.0f),
        .startSpeed = 1.2f,
        .endSpeed = 0.0f,
        .startScale = 0.025f,
        .endScale = 0.0f,
        .decayTime = 2.58f,
        .randomTimeOffsetDist = 2.58f,
        .looping = 1,
        .emitterType = 1,
        .discRadius = 0.020f
    };
    this->particleEmitterRight = new ParticleEmitter(numParticles);
    this->particleEmitterRight->data = this->particleEmitterLeft->data;

    ParticleSystem::Instance()->AddEmitter(this->particleEmitterLeft);
    ParticleSystem::Instance()->AddEmitter(this->particleEmitterRight);


}

int
Podracer::Update(float dt, std::vector<Tile>& tiles)
{
    //Mouse* mouse = Input::GetDefaultMouse();
    Keyboard* kbd = Input::GetDefaultKeyboard();

    Camera* cam = CameraManager::GetCamera(CAMERA_MAIN);


    if(!disableControls){
        if(kbd->pressed[Key::R]){
            reset();
            return 1;
        }
        cameraX = kbd->held[Key::Q] ? 5.0f : kbd->held[Key::E] ? -5.0f : 0.0f;
        cameraY = kbd->held[Key::T] ? 5.0f : 0.0f;

        if(kbd->pressed[Key::Space]){
            automatic = !automatic;
        }
        if(!automatic){
            if (kbd->held[Key::W])
                this->currentSpeed = 1.f;

            else if (kbd->held[Key::S] && movementIndex >= 0){
                this->currentSpeed = -1.f;
            }
            else
                this->currentSpeed = 0;
        }
        else{
            this->currentSpeed = 1.f;
        }

        if (kbd->held[Key::A] && this->positionX >= -5) {
            this->currentSideSpeed = mix(this->currentSideSpeed, this->boostSpeed, std::min(1.0f, dt * 20.0f));
        }
        else if (kbd->held[Key::D] && this->positionX <= 5) {
            this->currentSideSpeed = mix(this->currentSideSpeed, -this->boostSpeed, std::min(1.0f, dt * 20.0f));
        }
        else {
            this->currentSideSpeed = 0;
        }


    }
    else{
        this->currentSpeed = 0.f;
        this->currentSideSpeed = 0.f;
    }

    this->movementIndex += dt * 10.f * currentSpeed;

    vec3 desiredVelocity = vec3(this->currentSideSpeed, 0, this->currentSpeed);
    desiredVelocity = this->transform * vec4(desiredVelocity, 0.0f);

    this->rotationY = tiles[(int)movementIndex].rotationY;
    float upcomingRotationY = tiles[(int)movementIndex+5].rotationY;
    if(rotationY == 45 || upcomingRotationY == 45)
        cameraY = 5;
    else if(rotationY == -45 || upcomingRotationY == -45)
        cameraY = 5;
    
    this->positionX += desiredVelocity.x * dt * 10.0f;
    this->racerPos = tiles[(int)movementIndex].position + vec3(0, 1 - abs(sin(this->rotationY)), 0);
    this->racerPos += vec3(positionX, 0, 0);
    //std::cout << racerPos.x << " " << racerPos.y << " " << racerPos.z << std::endl;

    this->transform = (tiles[(int)movementIndex].transform * translate(vec3(this->positionX, 0, 0))); //handles rotation of vehicle, affects movement direction

    // update camera view transform

    vec3 center = vec3(this->transform[3]) + vec3(0, 0, 5);
    float eyeX = this->transform[3].x + cameraX;
    float eyeY = this->transform[3].y + 1.5f + cameraY;
    float eyeZ = this->transform[3].z - 1.5f;
    vec3 eye = vec3(eyeX, eyeY, eyeZ);
    cam->view = lookAt(eye, center, vec3(0, 2, 0));

    //std::cout << "Center: " << (center).x << " " << (center).y << " " << (center).z << std::endl;
    //std::cout << "View: " << vec3(cam->view[2]).x << " " << vec3(cam->view[2]).y << " " << vec3(cam->view[2]).z << std::endl;
    //std::cout << "Orientation: " << this->orientation.x << " " << this->orientation.y << " " << this->orientation.z << std::endl;

    const float thrusterPosOffset = 0.365f;
    this->particleEmitterLeft->data.origin = glm::vec4(vec3(this->racerPos + (vec3(this->transform[0]) * -thrusterPosOffset)) + (vec3(this->transform[2]) * emitterOffset), 1);
    this->particleEmitterLeft->data.dir = glm::vec4(glm::vec3(-this->transform[2]), 0);
    this->particleEmitterRight->data.origin = glm::vec4(vec3(this->racerPos + (vec3(this->transform[0]) * thrusterPosOffset)) + (vec3(this->transform[2]) * emitterOffset), 1);
    this->particleEmitterRight->data.dir = glm::vec4(glm::vec3(-this->transform[2]), 0);
    
    float t = (currentSpeed / this->normalSpeed);
    this->particleEmitterLeft->data.startSpeed = 1.2 + (3.0f * t);
    this->particleEmitterLeft->data.endSpeed = 0.0f  + (3.0f * t);
    this->particleEmitterRight->data.startSpeed = 1.2 + (3.0f * t);
    this->particleEmitterRight->data.endSpeed = 0.0f + (3.0f * t);

    return 0;
    //this->particleEmitter->data.decayTime = 0.16f;//+ (0.01f  * t);
    //this->particleEmitter->data.randomTimeOffsetDist = 0.06f;/// +(0.01f * t);
}

bool
Podracer::CheckCollisions()
{
    bool hit = false;
    for (int i = 0; i < 8; i++)
    {
        glm::vec3 pos = racerPos;

        glm::vec3 dir = glm::vec4(glm::normalize(colliderEndPoints[i]), 0.0f);
        float len = glm::length(colliderEndPoints[i]);
        Physics::RaycastPayload payload = Physics::Raycast(pos, dir, len);

        // debug draw collision rays
        //Debug::DrawLine(pos, pos + dir * len, 2.0f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 0, 0, 1), Debug::RenderMode::AlwaysOnTop);

        if (payload.hit)
        {
            Debug::DrawDebugText("HIT", payload.hitPoint, glm::vec4(1, 1, 1, 1));
            hit = true;
        }
    }
    return hit;
}
void Podracer::reset(){
    automatic = false;
    movementIndex = 0;
    positionX = 0;
    racerPos = glm::vec3(0.f, 1.0f, 0.0f);
}
}