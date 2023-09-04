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

#define NANOVG_GL2_IMPLEMENTATION
#include "nanovg_gl.h"
#include "core/savefile.h"


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
        this->window->SetSize(640, 480);

        std::cout << "Choose a difficulty from 1 to 3, 1 being easy, 2 being medium, and 3 hardest";
        std::cout << "\nDifficulty: ";
        std::cin >> difficulty;

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
            LoadModel("assets/podracer/machine_barrelLarge.glb"),
            LoadModel("assets/podracer/satelliteDish.glb"),
            LoadModel("assets/podracer/barrel.glb"),
            LoadModel("assets/podracer/chimney.glb")
        };

        float colliderSize[10] = {
                1,
                1,
                1.7f,
                1.7f,
                0.7f,
                2,
                1.7f,
                1.5f,
                0.5f,
                0.9f


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

        const int amountOfPlanes = 1600;

        std::vector<Tile>tiles;

        int lastTileType = 1;
        //setup all tiles
        while(tiles.size() < amountOfPlanes){
            if(tiles.size() >= amountOfPlanes - 30){
                int difference = amountOfPlanes - tiles.size();
                for(int i=0; i<difference; i++){
                    createStraight(tiles, tiles.size());
                }
                break;
            }
            int tileType;
            if(lastTileType != 0){ //if slope
                tileType = 0; //straight
            }
            else{
                tileType = Core::TrueRandom(1, 2);
            }
            //Decide how many tiles are placed of selected type
            int tileNumber;
            if(tiles.size() <= 0){
                tileNumber = 30;
            }
            else{
                tileNumber = Core::TrueRandom(5, 30);
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
        glm::vec3 scaleBig;
        //glm::vec3 scaleSmol(3.f);
        glm::vec3 colScales;
        int xIndex;
        float span;
        switch(difficulty){
            case 1:
                span = 10;
                break;
            case 2:
                span = 6;
                break;
            case 3:
                span = 4;
                break;
            default:
                span = 10;
        }

        // Setup terrain
        for (int i = 10; i < (int)(amountOfPlanes / span); i++)
        {
            std::tuple<ModelId, Physics::ColliderId, glm::mat4, int> podModel; //Model to be created
            int resourceIndex = Core::TrueRandom(0, 9); //Randomizes which model
            Physics::ColliderMeshId col = boxMesh;
            //int resourceIndex = 8;
            if(resourceIndex == 0) { //if bones
                xIndex = 0.f; //bones spawn in middle of road
                col = boneCol;
                scaleBig = glm::vec3(17.5f);
                colScales = scaleBig;
            }

            else{
                xIndex = Core::TrueRandom(-7, 7);
                colScales = glm::vec3(colliderSize[resourceIndex]);
                scaleBig = glm::vec3(4.f);
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
            std::get<1>(podModel) = Physics::CreateCollider(col, glm::scale(glm::translate(glm::vec3(-position.x, position.y, position.z)), colScales) * tiles[(int)(i * span)].rotation);
            std::get<2>(podModel) = glm::scale(transform, scaleBig);
            std::get<3>(podModel) = id;
            asteroids.push_back(podModel);
        }

        // game loop

        bool collided = false;

        //std::chrono::high_resolution_clock::duration totalTime(0);
        auto start = std::chrono::high_resolution_clock::now();
        float timer = 0;
        bool timerUp = false;
        clock_t current_ticks, delta_ticks;
        clock_t fps = 0;
        float frameTime = 0;
        std::chrono::duration<double> diff;
        float previousTime = 0;
        struct NVGcontext* vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
        if(!vg){
            glfwTerminate();
            return;
        }

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
            {

                for (int i = ship.movementIndex - 10; i < ship.movementIndex + 40; i++) {
                    if (i < 0)
                        i = 0;
                    if (i >= amountOfPlanes) {
                        i = amountOfPlanes - 1;
                        break;
                    }
                    RenderDevice::Draw(std::get<0>(groundPlane), tiles[i].transform);
                    for (auto const& asteroid : asteroids) { //spawn terrain
                        if(i == std::get<3>(asteroid))
                            RenderDevice::Draw(std::get<0>(asteroid), std::get<2>(asteroid));
                    }
                }
            }
            //std::cout << ship.movementIndex << std::endl;

            if(seconds == 3){
                ship.automatic = true;
                ship.disableControls = false;
            }
            else if(seconds < 3){
                ship.automatic = false;
                ship.disableControls = true;
            }

            if(ship.Update(dt, tiles)){
                //if reset
                points = 0;
                start = std::chrono::high_resolution_clock::now();
                ship.automatic = false;
                ship.disableControls = true;
                won = false;
                saved = false;
            }
            if(ship.movementIndex >= tiles.size()-1){
                won = true;
            }
            const auto end = std::chrono::high_resolution_clock::now();

            seconds = (int)diff.count();
            if(diff.count() != 0)
            {
                previousTime = diff.count();
            }
            diff = end - start;
            frameTime = diff.count() - previousTime;
            if(!won){
                progress = (int)((ship.movementIndex / amountOfPlanes) * 100);
                this->collisionsOn = !ship.disableCollisions;
                collided = ship.CheckCollisions();

                if(!ship.disableCollisions)
                { //Collisions and respawning
                    if (collided && renderCar) //Game over
                    {
                        //std::cout << "OUCH" << std::endl;
                        renderCar = false;
                        timer = seconds + 3;
                        ship.disableControls = true;
                        ship.automatic = false;
                        if(!saved)
                            saveScore(points);
                        saved = true;
                    }
                }
                if(seconds >= timer && !renderCar)
                    timerUp = true;

                if(timerUp){
                    ship.reset();
                    points = 0;
                    start = std::chrono::high_resolution_clock::now();
                    renderCar = true;
                    ship.disableControls = false;
                    timerUp = false;
                    saved = false;
                }

                if(renderCar) {
                    RenderDevice::Draw(ship.model, ship.transform);
                    if(ship.automatic)
                        points = (int)( (100 * (diff.count()-3)) / (span));
                }
            }
            else{
                progress = 100;
                ship.disableControls = true;
                ship.automatic = false;
                if(!saved)
                    saveScore(points);
                saved = true;
                ship.movementIndex = 4;
            }

            // Execute the entire rendering pipeline
            RenderDevice::Render(this->window, dt);

            delta_ticks = clock() - current_ticks; //the time, in ms, that took to render the scene
            if(delta_ticks > 0)
                fps = CLOCKS_PER_SEC / delta_ticks;
            this->frames = 1 / frameTime;
            //std::cout << "Fps: " << (1 / frameTime) << std::endl;

            RenderNanoVG(vg);

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
    PodracerApp::RenderNanoVG(NVGcontext* vg) //TODO: Print tile progress on screen (x/1600), fix collisions
    {

        nvgBeginPath(vg);

        nvgFontSize(vg, 16.0f);
        nvgFontFace(vg, "sans");
        nvgFillColor(vg, nvgRGBA(255, 0, 0, 255));


        nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);

        //Text for frames (fps)
        char buf[100];
        sprintf(buf, "Fps: %f", frames);
        nvgText(vg, 550, 10, buf, NULL);

        //Text for collisions being on/off
        if(collisionsOn)
            nvgText(vg, 60, 460, "Collisions: ON", NULL);
        else
            nvgText(vg, 60, 460, "Collisions: OFF", NULL);

        //Text for points gained
        nvgFontSize(vg, 32.0f);
        char buf2[100];
        sprintf(buf2, "Points: %i", points);
        nvgText(vg, 320, 20, buf2, NULL);

        //Text for points gained
        sprintf(buf2, "Progress: %i%%", progress);
        nvgText(vg, 480, 440, buf2, NULL);

        //Text for game over
        if(!renderCar){
            prevPoints = loadScore();
            char buf[100];
            sprintf(buf, "%s%i", "Previous High Score:", prevPoints);
            nvgFontSize(vg, 50.0f);
            nvgText(vg, 320, 160, "GAME OVER", NULL);
            nvgText(vg, 320, 220, buf, NULL);
        }

        //Text for win
        if(won){
            prevPoints = loadScore();
            char buf[100];
            sprintf(buf, "%s%i", "Previous High Score:", prevPoints);
            nvgFontSize(vg, 50.0f);
            nvgText(vg, 320, 160, "YOU WIN!", NULL);
            nvgText(vg, 320, 220, buf, NULL);
        }

        //Text for countdown
        nvgFontSize(vg, 100.0f);
        if(seconds < 3){
            char buf3[100];
            sprintf(buf3, "%i", 3-seconds);
            nvgText(vg, 320, 180, buf3, NULL);
        }
        else if(seconds == 3){
            nvgText(vg, 320, 180, "GO!", NULL);
        }

        nvgRestore(vg);
    }

}