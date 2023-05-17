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

        std::vector<float>groundPos;
        groundPos.reserve(10);

        // game loop
        while (this->window->IsOpen())
        {
            auto timeStart = std::chrono::steady_clock::now();
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);

            this->window->Update();

            if (kbd->pressed[Input::Key::Code::End])
            {
                ShaderResource::ReloadShaders();
            }

            if(renderCar)
                ship.Update(dt);

            collided = ship.CheckCollisions();


            if(ship.position != prevPos){
                if(ship.position.z >= groundPos[6])
                {
                    prevPos = ship.position;
                    std::cout << groundPos[8] << std::endl;

                }
            }

            for (int i=0; i<10; i++) {
                glm::vec3 translation = glm::vec3(
                        0.0f, 0.f, prevPos.z + (i * 6)
                );
                groundPos[i] = translation.z;
                glm::mat4 transform = glm::translate(translation);
                RenderDevice::Draw(std::get<0>(groundPlane), transform);
            }


            //RenderDevice::Draw(std::get<0>(groundPlane), std::get<2>(groundPlane));

            if (!collided && renderCar)
            {
                RenderDevice::Draw(ship.model, ship.transform);               
            }
            else if (collided && renderCar) {
                //renderCar = false;
                ship.position = originalPos;
                ship.orientation = ori;
            }

            //Render::LightServer::SetPosition(lights[0], ship.position);
            //Render::LightServer::Update();

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