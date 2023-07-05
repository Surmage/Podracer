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

void
Podracer::Update(float dt, int& i, std::vector<Tile>& tiles)
{
    //Mouse* mouse = Input::GetDefaultMouse();
    Keyboard* kbd = Input::GetDefaultKeyboard();

    Camera* cam = CameraManager::GetCamera(CAMERA_MAIN);

    if (kbd->held[Key::W])
    {
        /*if (kbd->held[Key::Shift])
            this->currentSpeed = mix(this->currentSpeed, this->boostSpeed, std::min(1.0f, dt * 30.0f));
        else*/
        {
            this->movementIndex += dt * 10.f;

            this->currentSpeed = 1.f;

            /*if(tiles[(int)movementIndex].rotationY < 0.f){
                this->currentSpeed = sin(glm::radians(45.f));
                this->currentUpSpeed = sin(glm::radians(45.f));
                std::cout << "Bleh" << movementIndex << std::endl;
            }
            else if(tiles[(int)movementIndex].rotationY > 0.f){
                this->currentSpeed = sin(glm::radians(45.f));
                this->currentUpSpeed = -sin(glm::radians(45.f));
                std::cout << "Bleh" << movementIndex << std::endl;
            }
            else{
                this->currentUpSpeed = 0.f;
            }*/
            //this->position += normalize(tile.position);


        }
            //this->currentSpeed = mix(this->currentSpeed, this->normalSpeed, std::min(1.0f, dt * 90.0f));

    }
    else if (kbd->held[Key::S]) {       
            //this->currentSpeed = mix(this->currentSpeed, -(this->normalSpeed), std::min(1.0f, dt * 90.0f));
        this->movementIndex -= dt * 10.f;

        this->currentSpeed = -1.f;

        /*if(tiles[(int)movementIndex].rotationY < 0.f){
            this->currentSpeed = -sin(glm::radians(45.f));
            this->currentUpSpeed = -sin(glm::radians(45.f));
            std::cout << "Bleh" << movementIndex << std::endl;
        }
        else if(tiles[(int)movementIndex].rotationY > 0.f){
            this->currentSpeed = -sin(glm::radians(45.f));
            this->currentUpSpeed = sin(glm::radians(45.f));
            std::cout << "Bleh" << movementIndex << std::endl;
        }
        else{
            this->currentUpSpeed = 0.f;
        }*/
        //this->position = tile.position + vec3(0.f, 2.f, 0.f);
    }
    else
    {
        this->currentSpeed = 0;
        this->currentUpSpeed = 0;
    }
    if (kbd->held[Key::A]) {
        this->currentSideSpeed = mix(this->currentSideSpeed, this->boostSpeed, std::min(1.0f, dt * 20.0f));
    }
    else if (kbd->held[Key::D]) {
        this->currentSideSpeed = mix(this->currentSideSpeed, -this->boostSpeed, std::min(1.0f, dt * 20.0f));
    }
    else {
        this->currentSideSpeed = 0;
    }
    vec3 desiredVelocity = vec3(this->currentSideSpeed, this->currentUpSpeed, this->currentSpeed);
    desiredVelocity = this->transform * vec4(desiredVelocity, 0.0f);

    this->linearVelocity = mix(this->linearVelocity, desiredVelocity, dt * accelerationFactor);

    //float rotX = kbd->held[Key::Left] ? 1.0f : kbd->held[Key::Right] ? -1.0f : 0.0f;
    //float rotY = kbd->held[Key::Up] ? -1.0f : kbd->held[Key::Down] ? 1.0f : 0.0f;
    //float rotZ = kbd->held[Key::A] ? -1.0f : kbd->held[Key::D] ? 1.0f : 0.0f;
    //float rotY = 0;

    this->position += desiredVelocity * dt * 10.0f;
    this->racerPos = this->position * orientation;

    //if(orientation.z <= 1)
    //   orientation.z += 0.01;
    


    //vec3 center = this->position + normalize(vec3(0, 0, rotationZ)) * vec3(2);

    //std::cout << normalize(center).x << " " << normalize(center).y << " " << normalize(center).z << std::endl;

   /* if (tile.rotationY != orientation.x) {
        float difference = -(1 + (glm::radians(tile.rotationY)));

        if (orientation.x > difference) {
            orientation.x = difference;
            
        }
    }*/


    std::cout << movementIndex << std::endl;
    //std::cout << orientation.x << std::endl;
    //std::cout << this->position.x << " " << this->position.y << " " << this->position.z << std::endl;

    const float rotationSpeed = 1.8f * dt;
    //rotXSmooth = mix(rotXSmooth, rotX * rotationSpeed, dt * cameraSmoothFactor);
    //rotYSmooth = mix(rotYSmooth, rotY * rotationSpeed, dt * cameraSmoothFactor);
    //rotZSmooth = mix(rotZSmooth, rotZ * rotationSpeed, dt * cameraSmoothFactor);
    quat localOrientation = quat(vec3(rotYSmooth, rotXSmooth, rotZSmooth));

    float deg = 180;
    this->orientation = glm::rotate(glm::radians(deg), vec3(0, 1, 0));
    //this->orientation = vec3(-vec3(tile.transform[2]).y, 0, 0);
    
    //std::cout << this->linearVelocity.z << std::endl;

    this->rotationZ -= rotXSmooth;
    //this->rotationZ = clamp(this->rotationZ, -45.0f, 45.0f);
    //this->rotationZ = tiles[(int)movementIndex].rotationY;
    mat4 T = translate(this->racerPos);

    //this->transform = T * trans * (mat4)quat(vec3(0, 0, rotationZ));
    //this->transform = tile.transform * T;
    this->rotationZ = mix(this->rotationZ, 0.0f, dt * cameraSmoothFactor);
    //this->transform = T * (mat4)quat(vec3(sin(radians(tiles[(int)movementIndex].rotationY)), 0, 0)); //handles rotation of vehicle, affects movement direction
    float difference = tiles[(int)movementIndex+1].position.z - (int)movementIndex;


    this->transform = (tiles[(int)movementIndex].transform * translate(vec3(this->position.x, 0, 0))); //handles rotation of vehicle, affects movement direction

    //std::cout << T[0][0] << std::endl;
    //std::cout << position.x << " " << position.y << " " << position.z << std::endl;


    

    // update camera view transform
    //vec3 desiredCamPos = this->racerPos + vec3(0, camOffsetY, -2.0f);
    //this->position = mix(this->position, desiredCamPos, dt * cameraSmoothFactor);

    vec3 center = vec3(this->transform[3]) + vec3(0, 5 * -sin(radians(tiles[(int)movementIndex].rotationY)), 5);
    cam->view = lookAt(vec3(this->transform[3].x, this->transform[3].y + 1.5f + sin(radians(tiles[(int)movementIndex].rotationY)) , this->transform[3].z - 1.5f), center, vec3(0, 2, 0));
    //this->transform = mat4(quat(this->position + vec3(0, 0, 2)));
    //this->racerPos = this->position + vec3(0, 0, 5);

    //vec3 losDirection = normalize(center - position) + vec3(this->transform[2]); //figure this out
    //std::cout << "Center: " << (center).x << " " << (center).y << " " << (center).z << std::endl;
    //std::cout << "View: " << vec3(cam->view[2]).x << " " << vec3(cam->view[2]).y << " " << vec3(cam->view[2]).z << std::endl;
    //std::cout << "Orientation: " << this->orientation.x << " " << this->orientation.y << " " << this->orientation.z << std::endl;

    //vec3 offsetCenter = center + vec3(cam->view[2]);
    //this->racerPos = center + vec3(0, -camOffsetY, 2.0f);
    //this->racerPos = center - normalize(vec3(0, center.y, 0));
    //this->racerPos = center * vec3(cam->view[2]);
    //std::cout << rotX << " " << rotY << std::endl;
    /*if(rotX != 0){
        this->racerPos = center + vec3(0, -center.y/2, -vec3(cam->view[2]).y);
    }
    else if(rotY != 0){
        this->racerPos = center + vec3(0, vec3(cam->view[2]).z, -vec3(cam->view[2]).y);
    }
    else{
        this->racerPos = center + vec3(0, vec3(cam->view[2]).z, -vec3(cam->view[2]).y);
    }*/
    //this->racerPos = center + vec3(0, vec3(cam->view[2]).z, -vec3(cam->view[2]).y);
    //this->racerPos = center + vec3(0, -center.y/2, -vec3(cam->view[2]).y);

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
    //this->particleEmitter->data.decayTime = 0.16f;//+ (0.01f  * t);
    //this->particleEmitter->data.randomTimeOffsetDist = 0.06f;/// +(0.01f * t);
}

bool
Podracer::CheckCollisions()
{
    glm::mat4 rotation = (glm::mat4)orientation;
    bool hit = false;
    for (int i = 0; i < 8; i++)
    {
        glm::vec3 pos = racerPos;
        glm::vec3 dir = rotation * glm::vec4(glm::normalize(colliderEndPoints[i]), 0.0f);
        float len = glm::length(colliderEndPoints[i]);
        Physics::RaycastPayload payload = Physics::Raycast(racerPos, dir, len);

        // debug draw collision rays
        // Debug::DrawLine(pos, pos + dir * len, 1.0f, glm::vec4(0, 1, 0, 1), glm::vec4(0, 1, 0, 1), Debug::RenderMode::AlwaysOnTop);

        if (payload.hit)
        {
            Debug::DrawDebugText("HIT", payload.hitPoint, glm::vec4(1, 1, 1, 1));
            hit = true;
        }
    }
    return hit;
}
}