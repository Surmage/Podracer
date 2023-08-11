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
                    0.0f, 0.f, -1.f
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

    double clockToMilliseconds(clock_t ticks){
        // units/(units/time) => time (seconds) * 1000 = milliseconds
        return (ticks/(double)CLOCKS_PER_SEC)*1000.0;
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

        // load all terrain resources
        ModelId models[10] = {
            LoadModel("assets/podracer/bones.glb"),
            LoadModel("assets/podracer/barrels.glb"),
            LoadModel("assets/podracer/rock_largeA.glb"),
            LoadModel("assets/podracer/rock_largeB.glb"),
            LoadModel("assets/podracer/rover.glb"),
            LoadModel("assets/podracer/meteor.glb"),
            LoadModel("assets/podracer/rail.glb"),
            LoadModel("assets/podracer/satelliteDish.glb"),
            LoadModel("assets/podracer/barrel.glb"),
            LoadModel("assets/podracer/chimney.glb")
 
        };
        Physics::ColliderMeshId boneCol = Physics::LoadColliderMesh("assets/podracer/bones.glb");
        Physics::ColliderMeshId boxMesh = Physics::LoadColliderMesh("newassets/box_collider.glb");
      

        std::vector<std::tuple<ModelId, Physics::ColliderId, glm::mat4, int>> asteroids;


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

        glm::vec3 scaleBig(4.f, 4.f, 4.f);
        glm::vec3 scaleSmol(0.7f);
        glm::vec3 colScales;
        int xIndex;
        const float span = 10.0f;

        // Setup terrain
        for (int i = 2; i < (int)(1600 / span); i++)
        {
            std::tuple<ModelId, Physics::ColliderId, glm::mat4, int> podModel; //Model to be created
            int resourceIndex = Core::TrueRandom(0, 9); //Randomizes which model
            Physics::ColliderMeshId col = boxMesh;
            //int resourceIndex = 8;
            if(resourceIndex == 0) { //if bones
                xIndex = 0.f; //bones spawn in middle of road
                col = boneCol;
                colScales = scaleBig;
            }

            else{
                xIndex = Core::TrueRandom(-7, 7);
                colScales = scaleSmol;
            }

            std::get<0>(podModel) = models[resourceIndex];

            int extra = 0;

            if(tiles[(int)(i * span)+2].rotationY != tiles[(int)(i * span)].rotationY) //if next tile is different
                extra = -2;
            if(tiles[(int)(i * span)-2].rotationY != tiles[(int)(i * span)].rotationY) //if prev tile is different
                extra = 2;

            glm::vec3 position = glm::vec3(
                xIndex,
                tiles[(int)(i * span)+extra].position.y,
                tiles[(int)(i * span)+extra].position.z
            );
            int id = (int)(i * span)+extra;

            glm::mat4 transform = translate(position) * tiles[(int)(i * span)].rotation;
            std::get<1>(podModel) = Physics::CreateCollider(col, glm::scale(glm::translate(glm::vec3(-position.x, position.y, position.z)), colScales));
            std::get<2>(podModel) = glm::scale(transform, scaleBig);
            std::get<3>(podModel) = id;
            asteroids.push_back(podModel);
        }

        // game loop

        bool collided = false;
        bool renderCar = true;

        //std::chrono::high_resolution_clock::duration totalTime(0);
        auto start = std::chrono::high_resolution_clock::now();
        float timer = 0;
        bool timerUp = false;
        clock_t current_ticks, delta_ticks;
        clock_t fps = 0;

        while (this->window->IsOpen()) {
            auto timeStart = std::chrono::steady_clock::now();
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);

            current_ticks = clock();

            this->window->Update();

            if (kbd->pressed[Input::Key::Code::End]) {
                ShaderResource::ReloadShaders();
            }

            //Spawn tiles
            //TODO: Multithread
            {
                for (int i = ship.movementIndex - 10; i < ship.movementIndex + 40; i++) {
                    if (i < 0)
                        i = 0;
                    RenderDevice::Draw(std::get<0>(groundPlane), tiles[i].transform);
                    for (auto const& asteroid : asteroids) { //spawn terrain
                        if(i == std::get<3>(asteroid))
                            RenderDevice::Draw(std::get<0>(asteroid), std::get<2>(asteroid));
                    }
                }
            }

            if(ship.Update(dt, tiles)) //if reset
                start = std::chrono::high_resolution_clock::now();

            const auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff = end - start;

            collided = ship.CheckCollisions();

            points = (int)diff.count(); //Points based on time alive
            //std::cout << points << std::endl;


            glm::vec3 translation = glm::vec3(
                    1.5f, 0.5f, 2.f
            );
            glm::vec3 rotationAxis = normalize(glm::vec3(1.f, 0.f, 0.f));

            { //Collisions and respawning
                /*if (collided && renderCar)
                {
                    std::cout << "OUCH" << std::endl;
                    renderCar = false;
                    timer = points + 3;
                    ship.disableControls = true;
                    ship.automatic = false;
                }
                if(points >= timer && !renderCar)
                    timerUp = true;

                if(timerUp){
                    ship.reset();
                    start = std::chrono::high_resolution_clock::now();
                    renderCar = true;
                    ship.disableControls = false;
                    timerUp = false;
                }*/
            }



            if(renderCar)
                RenderDevice::Draw(ship.model, ship.transform);


            /*else if (collided && renderCar) {
                //renderCar = false;
                ship.position = originalPos;
                ship.orientation = ori;
            }*/

            // Execute the entire rendering pipeline
            RenderDevice::Render(this->window, dt);

            delta_ticks = clock() - current_ticks; //the time, in ms, that took to render the scene
            if(delta_ticks > 0)
                fps = CLOCKS_PER_SEC / delta_ticks;
            std::cout << "Fps: " << delta_ticks << std::endl;

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
    void
    PodracerApp::RenderNanoVGFPS(NVGcontext* vg, clock_t fps)
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
        nvgText(vg, 0, 30, "Fps " + fps, NULL);

        nvgRestore(vg);
    }
}