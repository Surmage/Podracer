#include "config.h"
#include "podracerapp.h"
#include "config.h"
#include <cstring>
#include "imgui.h"
#include "render/renderdevice.h"
#include "render/shaderresource.h"
#include <vector>
#include "render/textureresource.h"
#include "render/model.h"
#include "render/cameramanager.h"
#include "render/lightserver.h"
#include "render/debugrender.h"
#include "core/random.h"
#include "render/input/inputserver.h"
#include "core/cvar.h"
#include "render/physics.h"
#include <chrono>
#include "podracer.h"
#include <iostream>

using namespace Display;
using namespace Render;

namespace Game {

//------------------------------------------------------------------------------
/**
*/
    PodracerApp::PodracerApp() {
        // empty
    }

//------------------------------------------------------------------------------
/**
*/
    PodracerApp::~PodracerApp() {
        // empty
    }

    bool PodracerApp::Open() {
        App::Open();
        this->window = new Display::Window;
        this->window->SetSize(1280, 720);

        if (this->window->Open())
        {
            // set clear color to gray
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

            RenderDevice::Init();

            // set ui rendering function
            this->window->SetUiRender([this]()
                                      {
                                          this->RenderUI();
                                      });
            this->window->SetNanoFunc([this](NVGcontext* vg)
                                      {
                                          this->RenderNanoVG(vg);
                                      });

            return true;
        }
        return false;
    }

    void PodracerApp::Run() {
        int w;
        int h;
        this->window->GetSize(w, h);
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), float(w) / float(h), 0.01f, 1000.f);
        Camera* cam = CameraManager::GetCamera(CAMERA_MAIN);
        cam->projection = projection;

        //// load all resources
        //ModelId models[6] = {
        //    LoadModel("assets/space/Asteroid_1.glb"),
        //    LoadModel("assets/space/Asteroid_2.glb"),
        //    LoadModel("assets/space/Asteroid_3.glb"),
        //    LoadModel("assets/space/Asteroid_4.glb"),
        //    LoadModel("assets/space/Asteroid_5.glb"),
        //    LoadModel("assets/space/Asteroid_6.glb")
        //};
        //Physics::ColliderMeshId colliderMeshes[6] = {
        //    Physics::LoadColliderMesh("assets/space/Asteroid_1_physics.glb"),
        //    Physics::LoadColliderMesh("assets/space/Asteroid_2_physics.glb"),
        //    Physics::LoadColliderMesh("assets/space/Asteroid_3_physics.glb"),
        //    Physics::LoadColliderMesh("assets/space/Asteroid_4_physics.glb"),
        //    Physics::LoadColliderMesh("assets/space/Asteroid_5_physics.glb"),
        //    Physics::LoadColliderMesh("assets/space/Asteroid_6_physics.glb")
        //};

        

        //std::vector<std::tuple<ModelId, Physics::ColliderId, glm::mat4>> asteroids;



        //// Setup asteroids near
        //for (int i = 0; i < 0; i++)
        //{
        //    std::tuple<ModelId, Physics::ColliderId, glm::mat4> asteroid;
        //    size_t resourceIndex = (size_t)(Core::FastRandom() % 6);
        //    std::get<0>(asteroid) = models[resourceIndex];
        //    float span = 20.0f;
        //    glm::vec3 translation = glm::vec3(
        //        Core::RandomFloatNTP() * span,
        //        Core::RandomFloatNTP() * span,
        //        Core::RandomFloatNTP() * span
        //    );
        //    glm::vec3 rotationAxis = normalize(translation);
        //    float rotation = translation.x;
        //    glm::mat4 transform = glm::rotate(rotation, rotationAxis) * glm::translate(translation);
        //    std::get<1>(asteroid) = Physics::CreateCollider(colliderMeshes[resourceIndex], transform);
        //    std::get<2>(asteroid) = transform;
        //    asteroids.push_back(asteroid);
        //}

        //// Setup asteroids far
        //for (int i = 0; i < 0; i++)
        //{
        //    std::tuple<ModelId, Physics::ColliderId, glm::mat4> asteroid;
        //    size_t resourceIndex = (size_t)(Core::FastRandom() % 6);
        //    std::get<0>(asteroid) = models[resourceIndex];
        //    float span = 80.0f;
        //    glm::vec3 translation = glm::vec3(
        //        Core::RandomFloatNTP() * span,
        //        Core::RandomFloatNTP() * span,
        //        Core::RandomFloatNTP() * span
        //    );
        //    glm::vec3 rotationAxis = normalize(translation);
        //    float rotation = translation.x;
        //    glm::mat4 transform = glm::rotate(rotation, rotationAxis) * glm::translate(translation);
        //    std::get<1>(asteroid) = Physics::CreateCollider(colliderMeshes[resourceIndex], transform);
        //    std::get<2>(asteroid) = transform;
        //    asteroids.push_back(asteroid);
        //}

        ModelId plane = LoadModel("assets/podracer/plane.glb");
        Physics::ColliderMeshId planeMesh = Physics::LoadColliderMesh("assets/podracer/plane_physics.glb");
        std::tuple<ModelId, Physics::ColliderId, glm::mat4> groundPlane;

        //Setup plane
        {
            std::tuple<ModelId, Physics::ColliderId, glm::mat4> ground;
            std::get<0>(ground) = plane;
            glm::vec3 translation = glm::vec3(
                0.f, 0.f, 0.f
            );

            glm::mat4 transform = glm::translate(translation);
            std::get<1>(ground) = Physics::CreateCollider(planeMesh, transform);
            std::get<2>(ground) = transform;
            groundPlane = (ground);
        }

        //Just a guy
        ModelId alien = LoadModel("assets/podracer/alien.glb");



        // Setup skybox
        std::vector<const char*> skybox
        {
            "assets/space/bg.png",
            "assets/space/bg.png",
            "assets/space/bg.png",
            "assets/space/bg.png",
            "assets/space/bg.png",
            "assets/space/bg.png"
        };
        TextureResourceId skyboxId = TextureResource::LoadCubemap("skybox", skybox, true);
        RenderDevice::SetSkybox(skyboxId);

        Input::Keyboard* kbd = Input::GetDefaultKeyboard();

        const int numLights = 1;
        Render::PointLightId lights[numLights];
        // Setup lights
        for (int i = 0; i < numLights; i++)
        {
            glm::vec3 translation = glm::vec3(
                Core::RandomFloatNTP() * 20.0f,
                Core::RandomFloatNTP() * 20.0f,
                Core::RandomFloatNTP() * 20.0f
            );
            glm::vec3 color = glm::vec3(
                Core::RandomFloat(),
                Core::RandomFloat(),
                Core::RandomFloat()
            );
            lights[i] = Render::LightServer::CreatePointLight(translation, color, Core::RandomFloat() * 4.0f, 1.0f + (15 + Core::RandomFloat() * 10.0f));
        }

        SpaceShip ship;
        ship.model = LoadModel("assets/podracer/craft_racer.glb");
        glm::vec3 originalPos = ship.position;
        glm::vec3 prevPos = ship.position;
        glm::quat ori = ship.orientation;

        std::clock_t c_start = std::clock();
        double dt = 0.01667f;

        bool collided = false;
        bool renderCar = true;

        int amountOfPlanes = 1600;
        float planeL = 1.0f;


        std::vector<glm::mat4>planeTransforms;
        float height = 0.f;
        float rotation = 0;

        for (int i = 0;  i < amountOfPlanes; i++) {

            glm::vec3 translation;
            glm::vec3 rotationAxis;
            if(i > 50 && i < 300){
                //rotationAxis = normalize(glm::vec3(translation));
                rotationAxis = normalize(glm::vec3(1.f, 0.f, 0.f));
                rotation = -45;
                translation = glm::vec3(
                        0.0f, (height + 1.41f) / 2, (i * planeL)/2 //1.41 being sqrt of 1+1
                );
                height++;
            }
            else if(i > 300 && i < 700){
                rotationAxis = normalize(glm::vec3(-1.f, 0.f, 0.f));
                rotation = 45;
                translation = glm::vec3(
                        0.0f, (height - 1.41f) / 2, (i * planeL)/2 //1.41 being sqrt of 1+1
                );
                height--;
            }
            else{
                rotationAxis = normalize(glm::vec3(translation));
                rotation = 0.f;
                translation = glm::vec3(
                        0.0f, height, (i * planeL)
                );
            }

            glm::mat4 rotate = glm::rotate(glm::radians(rotation), rotationAxis);
            glm::mat4 translate = glm::translate(translation);
            glm::mat4 transform = translate * rotate;
            //glm::mat4 transform =  glm::translate(translation);
            planeTransforms.push_back(transform);
        }
        float rotamt = 0.f;

        // game loop
        while (this->window->IsOpen()) {
            rotamt += 0.001;
            auto timeStart = std::chrono::steady_clock::now();
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);

            this->window->Update();

            if (kbd->pressed[Input::Key::Code::End]) {
                ShaderResource::ReloadShaders();
            }

            //Spawn tiles
            {
                //for (int i = 0; i < 100; i++) {
                for (int i = ship.position.z; i < ship.position.z + 100; i++) {


                    //glm::mat4 transform = glm::rotate(rotation, rotationAxis) * glm::translate(translation);
                    RenderDevice::Draw(std::get<0>(groundPlane), planeTransforms[i]);
                }
            }

            if (renderCar)
            { //mat4, 4th matrix x y z
                //planeTransforms[ship.position.z];
                if(ship.position.z < 20){
                    ship.Update(dt, 0.f);
                }
                else{
                    ship.Update(dt, 45.f / 2);
                }


                //glm::mat4 m = planeTransforms[ship.position.z];

                //std::cout << m[3].x << " " <<  m[3].y << " " << m[3].z << std::endl;
                //glm::vec3 v = glm::vec3(m[3].x, m[3].y+5, m[3].z);


            }


            collided = ship.CheckCollisions();

            //std::cout << "Ship: " << ship.position.z << std::endl;

            glm::vec3 translation = glm::vec3(
                    1.5f, 1.5f, 2.f
            );
            glm::vec3 rotationAxis = normalize(glm::vec3(1.f, 0.f, 0.f));
            float rotation = rotamt;
            glm::mat4 alienTransform = glm::rotate(rotation, rotationAxis) * glm::translate(translation);

            if (!collided && renderCar)
            {
                RenderDevice::Draw(ship.model, ship.transform);
                RenderDevice::Draw(alien, alienTransform);
            }
            /*else if (collided && renderCar) {
                //renderCar = false;
                ship.position = originalPos;
                ship.orientation = ori;
            }*/

            // Execute the entire rendering pipeline
            RenderDevice::Render(this->window, dt);

            // transfer new frame to window
            this->window->SwapBuffers();

            auto timeEnd = std::chrono::steady_clock::now();
            dt = std::min(0.04, std::chrono::duration<double>(timeEnd - timeStart).count());

            if (kbd->pressed[Input::Key::Code::Escape])
                this->Exit();
        }
    }

    void PodracerApp::Exit() {

    }

    void
    PodracerApp::RenderUI()
    {
        if (this->window->IsOpen())
        {


            //Debug::DispatchDebugTextDrawing();
        }
    }

    void
    PodracerApp::RenderNanoVG(NVGcontext* vg)
    {
        nvgSave(vg);

        nvgBeginPath(vg);
        nvgCircle(vg, 600, 100, 50);
        NVGpaint paint;
        paint = nvgLinearGradient(vg, 600, 100, 650, 150, nvgRGBA(255, 0, 0, 255), nvgRGBA(0, 255, 0, 255));
        nvgFillPaint(vg, paint);
        nvgFill(vg);



        // Header
        nvgBeginPath(vg);
        nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 32));
        nvgStroke(vg);

        nvgFontSize(vg, 16.0f);
        nvgFontFace(vg, "sans");
        nvgFillColor(vg, nvgRGBA(255, 255, 255, 128));
        nvgText(vg, 0, 30, "Testing, testing... Everything seems to be in order.", NULL);

        nvgRestore(vg);
    }
}