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

    void createStraight(std::vector<Tile>& tiles, int i){
        glm::vec3 position;
        glm::vec3 rotationAxis;
        glm::mat4 rotate;
        glm::vec3 edge;
        float rotation = 0.f;
        if(i < 1){
            rotation = 0.f;
            position = glm::vec3(
                    0.0f, 0.f, 0.f
            );
            rotate = glm::mat4(1.f);
            edge = glm::vec3(position.x, position.y, position.z + 0.5f);
        }
        else{
            position = glm::vec3(
                    0.0f, tiles[i-1].edge.y, tiles[i-1].edge.z + 0.5f
            );
            rotationAxis = position;
            rotate = glm::mat4(1.f);
            edge = glm::vec3(0, position.y, position.z + 0.5f);
        }


        glm::mat4 translate = glm::translate(position);
        glm::mat4 transform = (translate * rotate) * (glm::mat4)glm::quat(glm::vec3(0, 3.14159f, 0)); //position, rotation, and flipping 180 degrees
        Tile t(position, transform, rotate, edge, tiles.size());
        t.rotationY = rotation;
        //glm::mat4 transform =  glm::translate(position);
        tiles.push_back(t);
    }
    void createInclineUp(std::vector<Tile>& tiles, int i){

        glm::vec3 position;
        glm::vec3 rotationAxis;
        glm::mat4 rotate;
        glm::vec3 edge;
        rotationAxis = normalize(glm::vec3(1.f, 0.f, 0.f));
        float rotation = -45;

        float angle = (sin(glm::radians(-rotation)));
        position = glm::vec3(
                0.0f, tiles[i-1].edge.y + (angle / 2), tiles[i-1].edge.z + (angle / 2)//1.41 being sqrt of 1+1
        ); //this fails? because the length of 1 is insufficient with an incline of 45 degrees (Pythagorean)

        rotate = glm::rotate(glm::radians(rotation), rotationAxis);
        edge = glm::vec3(0.f, position.y + (angle / 2), position.z + (angle / 2));

        glm::mat4 translate = glm::translate(position);
        glm::mat4 transform = (translate * rotate) * (glm::mat4)glm::quat(glm::vec3(0, 3.14159f, 0)); //position, rotation, and flipping 180 degrees
        Tile t(position, transform, rotate, edge, tiles.size());
        t.rotationY = rotation;
        //glm::mat4 transform =  glm::translate(position);
        tiles.push_back(t);
    }
    void createInclineDown(std::vector<Tile>& tiles, int i){
        glm::vec3 position;
        glm::vec3 rotationAxis;
        glm::mat4 rotate;
        glm::vec3 edge;
        rotationAxis = normalize(glm::vec3(1.f, 0.f, 0.f));
        float rotation = 45;

        float angle = (sin(glm::radians(rotation)));
        position = glm::vec3(
                0.0f, tiles[i-1].edge.y - (angle / 2), tiles[i-1].edge.z + (angle / 2)
        );

        rotate = glm::rotate(glm::radians(rotation), rotationAxis);
        edge = glm::vec3(0.f, position.y - (angle / 2), position.z + (angle / 2));

        glm::mat4 translate = glm::translate(position);
        glm::mat4 transform = (translate * rotate) * (glm::mat4)glm::quat(glm::vec3(0, 3.14159f, 0)); //position, rotation, and flipping 180 degrees
        Tile t(position, transform, rotate, edge, tiles.size());
        t.rotationY = rotation;
        //glm::mat4 transform =  glm::translate(translation);
        tiles.push_back(t);
    }

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

        //// load all terrain resources
        ModelId models[10] = {
            LoadModel("assets/podracer/barrels.glb"),
            LoadModel("assets/podracer/bones.glb"),
            LoadModel("assets/podracer/rock_largeA.glb"),
            LoadModel("assets/podracer/rock_largeB.glb"),
            LoadModel("assets/podracer/rover.glb"),
            LoadModel("assets/podracer/meteor.glb"),
            LoadModel("assets/podracer/rail.glb"),
            LoadModel("assets/podracer/satelliteDish.glb"),
            LoadModel("assets/podracer/barrel.glb"),
            LoadModel("assets/podracer/chimney.glb")
            //LoadModel("newassets/box_collider.glb")
 
        };
        Physics::ColliderMeshId boxMesh = Physics::LoadColliderMesh("newassets/box_collider.glb");
      

        std::vector<std::tuple<ModelId, Physics::ColliderId, glm::mat4>> asteroids;


        ModelId plane = LoadModel("newassets/plane.glb");
        Physics::ColliderMeshId planeMesh = Physics::LoadColliderMesh("newassets/plane_physics.glb");
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
            "newassets/whitebg.png",
            "newassets/whitebg.png",
            "newassets/whitebg.png",
            "newassets/whitebg.png",
            "newassets/whitebg.png",
            "newassets/whitebg.png"

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

        Podracer ship;
        ship.model = LoadModel("assets/podracer/craft_racer.glb");

        double dt = 0.01667f;

        int amountOfPlanes = 1600;

        std::vector<Tile>tiles;

        int lastTileType = 1;
        //setup all tiles
        while(tiles.size() < 1600){
            int tileType;
            if(lastTileType != 0){ //if slope
                tileType = 0; //straight
            }
            else{
                tileType = Core::TrueRandom(1, 2);
            }
            int tileNumber = Core::TrueRandom(5, 30);
            if(tiles.size() >= 1550){
                int difference = 1600 - tiles.size();
                for(int i=0; i<difference; i++){
                    createStraight(tiles, tiles.size());
                }
            }
            for(int i=0; i<tileNumber; i++){
                if(tileType == 0 || tiles.size() == 0){
                    createStraight(tiles, tiles.size());
                }
                else if(tileType == 1){
                    createInclineUp(tiles, tiles.size());
                }
                else if(tileType == 2){
                    createInclineDown(tiles, tiles.size());
                }
            }
            lastTileType = tileType;
        }
        std::cout << tiles.size() << std::endl;
        /*for (int i = 0;  i < amountOfPlanes; i++) {
            int tileType = Core::TrueRandom(0, 2);
            glm::vec3 translation;
            glm::vec3 rotationAxis;
            glm::mat4 rotate;
            glm::vec3 edge;
            if(tileType == 0){
                createStraight(tiles, i);
            }
            else if(tileType == 1){
                createInclineUp(tiles, i);
            }
            else if(tileType == 2){
                createInclineDown(tiles, i);
            }

            if(i == 0){
                createStraight(tiles, i);
            }
            else if(i > 50 && i < 100){
                createInclineUp(tiles, i);
            }
            else if(i > 130 && i < 200){
                createInclineDown(tiles, i);
            }
            else if(i > 225 && i < 250){
                createInclineDown(tiles, i);
            }
            else if(i > 275 && i < 290){
                createInclineUp(tiles, i);
            }
            else if(i > 300 && i < 350){
                createInclineDown(tiles, i);
            }
            else if(i > 360 && i < 400){
                createInclineUp(tiles, i);
            }
            else if(i > 410 && i < 440){
                createInclineDown(tiles, i);
            }
            else if(i > 460 && i < 500){
                createInclineUp(tiles, i);
            }
            else{
                createStraight(tiles, i);
            }

        }*/

        glm::vec3 scales(4.f, 4.f, 4.f);

        // Setup terrain
        for (int i = 1; i < 200; i++)
        {
            std::tuple<ModelId, Physics::ColliderId, glm::mat4> podModel;
            int resourceIndex = Core::TrueRandom(0, 9);
            int xIndex = Core::TrueRandom(-7, 7);
            std::get<0>(podModel) = models[resourceIndex];
            float span = 8.0f;
            int extra = 0;

            if(tiles[(int)(i * span)+1].rotationY != tiles[(int)(i * span)].rotationY) //if next tile is different
                extra = -1;
            else if(tiles[(int)(i * span)-1].rotationY != tiles[(int)(i * span)].rotationY) //if prev tile is different
                extra = 1;

            glm::vec3 translation = glm::vec3(
                xIndex,
                tiles[(int)(i * span)+extra].position.y,
                tiles[(int)(i * span)+extra].position.z
            );

            glm::vec3 rotationAxis = normalize(translation);
            float rotation = translation.x;
            glm::mat4 transform = translate(translation) * tiles[(int)(i * span)].rotation;
            std::get<1>(podModel) = Physics::CreateCollider(boxMesh, glm::scale(transform, scales));
            std::get<2>(podModel) = glm::scale(transform, scales);
            asteroids.push_back(podModel);
        }


        // game loop

        bool collided = false;
        bool renderCar = true;

        //std::chrono::high_resolution_clock::duration totalTime(0);
        auto start = std::chrono::high_resolution_clock::now();

        while (this->window->IsOpen()) {
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
                for (int i = ship.movementIndex - 10; i < ship.movementIndex + 100; i++) {
                    if (i < 0)
                        i = 0;
                    RenderDevice::Draw(std::get<0>(groundPlane), tiles[i].transform);
                }
            }
            if (renderCar){
                if(ship.Update(dt, tiles)) //if reset
                    start = std::chrono::high_resolution_clock::now();
            }
            const auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff = end - start;

            collided = ship.CheckCollisions();

            points = (int)diff.count();
            std::cout << points << std::endl;


            glm::vec3 translation = glm::vec3(
                    1.5f, 0.5f, 2.f
            );
            glm::vec3 rotationAxis = normalize(glm::vec3(1.f, 0.f, 0.f));

            if (collided)
            {
                std::cout << "OUCH" << std::endl;
                //renderCar = false;
                //ship.reset();
                //start = std::chrono::high_resolution_clock::now();
            }

            for (auto const& asteroid : asteroids) {
                RenderDevice::Draw(std::get<0>(asteroid), std::get<2>(asteroid));
            }
            RenderDevice::Draw(ship.model, ship.transform);


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
        //nvgCircle(vg, 600, 100, 50);
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
        nvgFillColor(vg, nvgRGBA(255, 0, 0, 128));
        nvgText(vg, 0, 30, "Testing, testing... Everything seems to be in order.", NULL);

        nvgRestore(vg);
    }
}