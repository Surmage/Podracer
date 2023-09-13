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
    bool gamepadOn;

void joystick_callback(int jid, int event)
{
    if (event == GLFW_CONNECTED)
    {
        // The joystick was connected
        gamepadOn = true;
    }
    else if (event == GLFW_DISCONNECTED)
    {
        // The joystick was disconnected
        gamepadOn = false;
    }
}

Podracer::Podracer()
{
    Gamepad* gamepad = Input::GetGamepad();
    gamepadOn = bool(glfwJoystickPresent(GLFW_JOYSTICK_1));
    gamepadAxis = gamepad->getAxis();
    gamepadButtons = gamepad->getButtons();
    glfwSetJoystickCallback(joystick_callback);

}

int
Podracer::Update(float dt, std::vector<Tile>& tiles)
{
    //Mouse* mouse = Input::GetDefaultMouse();
    Keyboard* kbd = Input::GetDefaultKeyboard();

    Gamepad* gamepad = Input::GetGamepad();
    gamepad->setAxis();
    gamepad->setButtons();

    Camera* cam = CameraManager::GetCamera(CAMERA_MAIN);

    if(movementIndex == 4) //If at start position
        if(kbd->pressed[Key::R] || (gamepadOn && gamepadButtons[3])){
            reset();
            return 1;
        }
    if(!disableControls) { //If controls on
        if(!gamepadOn){
            if (kbd->pressed[Key::R]) {
                reset();
                return 1;
            }
            //Toggle side views
            cameraX = kbd->held[Key::Q] ? 5.0f : kbd->held[Key::E] ? -5.0f : 0.0f;
            cameraY = kbd->held[Key::T] ? 5.0f : 0.0f;

            /*if(kbd->pressed[Key::Space]){
                automatic = !automatic;
            }*/
            if (!automatic) { //If on manual
                if (kbd->held[Key::W]) //Move forward
                    this->currentSpeed = 1.f;

                else if (kbd->held[Key::S] && movementIndex >= 0) { //Move backwards
                    this->currentSpeed = -1.f;
                } else //Stop forward movement
                    this->currentSpeed = 0;
            }
            else {
                this->currentSpeed = 1.f;
            }
            if (kbd->held[Key::A] && this->positionX >= -5) { //Move left
                this->currentSideSpeed = mix(this->currentSideSpeed, this->boostSpeed, std::min(1.0f, dt * 20.0f));
            } else if (kbd->held[Key::D] && this->positionX <= 5) { //Move right
                this->currentSideSpeed = mix(this->currentSideSpeed, -this->boostSpeed, std::min(1.0f, dt * 20.0f));
            } else { //Stop side movement
                this->currentSideSpeed = 0;
            }
        }
        else{
            if (gamepadAxis[0] < -0.1f && this->positionX >= -5) {
                this->currentSideSpeed = 1;
            }
            else if (gamepadAxis[0] > 0.1f && this->positionX <= 5) {
                this->currentSideSpeed = -1;
            }
            else {
                this->currentSideSpeed = 0;
            }
            //Toggle side views
            if(gamepadButtons[4]){
                cameraX = 5.f;
            }
            else if(gamepadButtons[5]){
                cameraX = -5.f;
            }
            else{
                cameraX = 0.f;
            }
            if(gamepadButtons[3]){
                reset();
                return 1;
            }

            this->cameraY = 0.f;
            this->currentSpeed = 1.f;
        }
    } else {
        this->currentSpeed = 0.f;
        this->currentSideSpeed = 0.f;
    }
    if (kbd->pressed[Key::C]) {
        disableCollisions = !disableCollisions;
    }



    //The transform is set to tile[movementIndex]'s later
    this->movementIndex += dt * 10.f * currentSpeed;

    vec3 desiredVelocity = vec3(this->currentSideSpeed, 0, this->currentSpeed);
    desiredVelocity = this->transform * vec4(desiredVelocity, 0.0f);

    //Check if 5 tiles ahead is within scope
    if (movementIndex+5 <= tiles.size()) {
        //Store the rotation of the current tile
        this->rotationY = tiles[(int)movementIndex].rotationY;
        //Store the rotation of the tile 5 spaces ahead
        this->upcomingRotationY = tiles[(int)movementIndex + 5].rotationY;
    }

    //Adjust camera based on tile rotations
    if(rotationY == 45 || upcomingRotationY == 45)
        this->cameraY = 5;
    else if(rotationY == -45 || upcomingRotationY == -45)
        this->cameraY = 5;
    
    this->positionX += desiredVelocity.x * dt * 10.0f; 
    this->racerPos = tiles[(int)movementIndex].position; //Set position to a tile's based on index
    this->racerPos += vec3(positionX, 0, 0); //Add movement in the x-axis
    this->transform = (tiles[(int)movementIndex].transform * translate(vec3(this->positionX, 0, 0))); //Tile transform * x position
    
    // update camera view transform
    vec3 center = vec3(this->transform[3]) + vec3(0, 0, 5);
    float eyeX = this->transform[3].x + this->cameraX;
    float eyeY = this->transform[3].y + 1.5f + this->cameraY;
    float eyeZ = this->transform[3].z - 1.5f;
    vec3 eye = vec3(eyeX, eyeY, eyeZ);
    cam->view = lookAt(eye, center, vec3(0, 2, 0));

    return 0;
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
    cameraY = 0;
    automatic = false;
    disableControls = true;
    movementIndex = 4;
    positionX = 0;
    racerPos = glm::vec3(0.f, 1.0f, 4.0f);
}
}