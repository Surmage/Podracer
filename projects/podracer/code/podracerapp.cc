#include "config.h"
#include "podracerapp.h"
#include "render/renderdevice.h"
#include "render/shaderresource.h"
#include <vector>
#include "render/textureresource.h"
#include "render/model.h"
#include "render/cameramanager.h"
#include "render/lightserver.h"
#include "core/random.h"
#include "render/input/inputserver.h"
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
        glm::mat4 rotate = glm::mat4(1.f);
        glm::vec3 edge;

        if(i < 1){ //if first tile
            position = glm::vec3(
                    0.0f, 0.f, -1.f
            );
        }
        else{
            position = glm::vec3( //place based on previous tile's y and z
                    //for y, use same y as previous
                    //for z, add 0.5 to the previous' edge, to get the correct center point
                    0.0f, tiles[i-1].edge.y, tiles[i-1].edge.z + 0.5f
            );
        }
        edge = glm::vec3(0, position.y, position.z + 0.5f); //set edge of newly made tile

        glm::mat4 translate = glm::translate(position); //only position, no rotation
        glm::mat4 transform = (translate) * (glm::mat4)glm::quat(glm::vec3(0, 3.14159f, 0)); //position, and flipping 180 degrees
        Tile t(position, transform, rotate, edge, static_cast<int>(tiles.size())); //Create tile with all the maths
        t.rotationY = 0.f; //No rotation
        tiles.push_back(t);
    }
    void createInclineUp(std::vector<Tile>& tiles, int i){
        glm::vec3 position;
        glm::vec3 rotationAxis;
        glm::mat4 rotate;
        glm::vec3 edge;

        rotationAxis = normalize(glm::vec3(1.f, 0.f, 0.f)); //Rotates around x axis
        float rotation = -45; 

        float angle = (sin(glm::radians(-rotation))); //Degrees to radians
        position = glm::vec3(
                //Position is based on previous tile's position,
                0.0f, tiles[i-1].edge.y + (angle / 2), tiles[i-1].edge.z + (angle / 2)
        );

        rotate = glm::rotate(glm::radians(rotation), rotationAxis);
        edge = glm::vec3(0.f, position.y + (angle / 2), position.z + (angle / 2));

        glm::mat4 translate = glm::translate(position);
        glm::mat4 transform = (translate * rotate) * (glm::mat4)glm::quat(glm::vec3(0, 3.14159f, 0)); //position, rotation, and flipping 180 degrees
        Tile t(position, transform, rotate, edge, static_cast<int>(tiles.size())); //Tile created
        t.rotationY = rotation; //Rotation of -45
        tiles.push_back(t);
    }
    void createInclineDown(std::vector<Tile>& tiles, int i){
        glm::vec3 position;
        glm::vec3 rotationAxis;
        glm::mat4 rotate;
        glm::vec3 edge;

        rotationAxis = normalize(glm::vec3(1.f, 0.f, 0.f)); //Rotates around x-axis
        float rotation = 45;

        float angle = (sin(glm::radians(rotation)));
        position = glm::vec3(
                //Maths for new position based on previous
                0.0f, tiles[i-1].edge.y - (angle / 2), tiles[i-1].edge.z + (angle / 2)
        );

        rotate = glm::rotate(glm::radians(rotation), rotationAxis);
        edge = glm::vec3(0.f, position.y - (angle / 2), position.z + (angle / 2));

        glm::mat4 translate = glm::translate(position);
        glm::mat4 transform = (translate * rotate) * (glm::mat4)glm::quat(glm::vec3(0, 3.14159f, 0)); //position, rotation, and flipping 180 degrees
        Tile t(position, transform, rotate, edge, static_cast<int>(tiles.size())); //Tile created
        t.rotationY = rotation;
        tiles.push_back(t);
    }

//------------------------------------------------------------------------------
/**
*/
    PodracerApp::PodracerApp() = default;

//------------------------------------------------------------------------------
/**
*/
    PodracerApp::~PodracerApp() = default;

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

        //Sizes for the boxes assigned to each model
        float colliderSize[10] = {
                1, //bones
                1, //barrels
                1.7f, //rock_largeA
                1.7f, //rock_largeB
                0.7f, //rover
                2, //meteor
                1.7f, //machine_barrelLarge
                1.5f, //satelliteDish
                0.5f, //barrel
                0.9f //chimney


        };
        //The bones model uniquely uses it's own model as its collider mesh
        Physics::ColliderMeshId boneCol = Physics::LoadColliderMesh("assets/podracer/bones.glb");
        //Other models for the terrain uses a box
        Physics::ColliderMeshId boxMesh = Physics::LoadColliderMesh("newassets/box_collider.glb");

        std::vector<std::tuple<ModelId, Physics::ColliderId, glm::mat4, int>> obstacles;

        //This tile is rectangular and is used to create the course, has a length of 1
        ModelId plane = LoadModel("newassets/plane.glb");
        //Plane model without its textures

        // Setup skybox
        std::vector<const char*> skybox
        {
            //White background to represent fog
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

        //One light is enough
        const int numLights = 1;
        Render::PointLightId lights[numLights];
        // Setup lights
        for (auto & light : lights)
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
            light = Render::LightServer::CreatePointLight(translation, color, Core::RandomFloat() * 4.0f, 1.0f + (15 + Core::RandomFloat() * 10.0f));
        }

        Podracer ship;
        ship.model = LoadModel("assets/podracer/craft_racer.glb");

        float dt = 0.01667f;

        //Number of tiles for the course, aka course length
        const int amountOfTiles = 1600;

        std::vector<Tile>tiles;

        int lastTileType = 1; //set to 1 so the course begins straight
        //setup all tiles
        while(tiles.size() < amountOfTiles){
            //If close to the maximum amount
            if(tiles.size() >= amountOfTiles - 30){
                //Find the difference to the max
                int difference = amountOfTiles - static_cast<int>(tiles.size());
                //Make the last stretch straight
                for(int i=0; i<difference; i++){
                    createStraight(tiles, static_cast<int>(tiles.size()));
                }
                break;
            }
            int tileType;
            if(lastTileType != 0){ //if slope
                tileType = 0; //straight
            }
            else{ //if straight
                tileType = Core::TrueRandom(1, 2); //Randomize up or down slope
            }
            //Decide how many tiles are placed of selected type
            int tileNumber;
            if(tiles.empty()){ //Start with 30
                tileNumber = 30;
            }
            else{
                tileNumber = Core::TrueRandom(5, 30); //Between 5 and 30
            }
            //Create the tiles for the tileNumber amount
            for(int i=0; i<tileNumber; i++){
                if(tileType == 0 || tiles.empty()){
                    createStraight(tiles, static_cast<int>(tiles.size()));
                }
                else if(tileType == 1){
                    createInclineUp(tiles, static_cast<int>(tiles.size()));
                }
                else if(tileType == 2){
                    createInclineDown(tiles, static_cast<int>(tiles.size()));
                }
            }
            lastTileType = tileType;
        }
        
        //Decide span between objects based on difficulty chosen, smaller span means less distance inbetween
        float span;
        switch (difficulty) {
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

        //Terrain/Obstacles
        {
            glm::vec3 scaleBig; //For upscaling obstacles
            glm::vec3 colScales; //For scaling collider
            int xIndex;

            // Setup terrain
            for (int i = 10; i < (int)(amountOfTiles / span); i++)
            {
                std::tuple<ModelId, Physics::ColliderId, glm::mat4, int> glbModel; //Model to be created
                int resourceIndex = Core::TrueRandom(0, 9); //Randomizes which model
                Physics::ColliderMeshId col = boxMesh;
                if (resourceIndex == 0) { //if bones
                    xIndex = 0.f; //bones spawn in middle of road
                    col = boneCol;
                    scaleBig = glm::vec3(17.5f); //Big bones
                    colScales = scaleBig; //Collider has same size
                }

                else { //If not bones model
                    xIndex = Core::TrueRandom(-7, 7); //Randomized x position
                    colScales = glm::vec3(colliderSize[resourceIndex]); //Get scale assigned to model for collider
                    scaleBig = glm::vec3(4.f); //Scale up model
                }

                std::get<0>(glbModel) = models[resourceIndex]; //Get the model based on number generated

                //Extra is used for if a model spawns at the end of a tile type set
                //This makes the big models avoid seemingly partially going off the ground
                int extra = 0;
                auto ii = static_cast<int>(span) * i;
                if (tiles[ii + 2].rotationY != tiles[ii].rotationY) //if next tile is different
                    extra = -2;
                if (tiles[ii - 2].rotationY != tiles[ii].rotationY) //if prev tile is different
                    extra = 2;
             
                glm::vec3 position = glm::vec3(
                    xIndex,
                    tiles[ii + extra].position.y,
                    tiles[ii + extra].position.z
                );
                int id = ii + extra;

                glm::mat4 transform = translate(position) * tiles[ii].rotation; //Position times rotation
                //Collider needs to be flipped in its x-axis position
                std::get<1>(glbModel) = Physics::CreateCollider(col, glm::scale(glm::translate(glm::vec3(-position.x, position.y, position.z)), colScales) * tiles[ii].rotation);
                std::get<2>(glbModel) = glm::scale(transform, scaleBig);
                std::get<3>(glbModel) = id;
                obstacles.push_back(glbModel);
            }
        }

        //game loop stuff

        bool collided = false;
        auto start = std::chrono::high_resolution_clock::now();
        float timer = 0; //used for respawning and countdown
        bool timerUp = false;
        clock_t current_ticks, delta_ticks;
        clock_t fps = 0;
        float frameTime = 0;
        std::chrono::duration<double> diff = std::chrono::seconds();
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
                //Spawn 40 tiles ahead of player
                for (int i = static_cast<int>(ship.movementIndex) - 10; i < static_cast<int>(ship.movementIndex) + 40; i++) {
                    //Limit i to not go out of bounds
                    if (i < 0)
                        i = 0;
                    if (i >= amountOfTiles) {
                        i = amountOfTiles - 1;
                        break;
                    }
                    RenderDevice::Draw(plane, tiles[i].transform);
                    for (auto const& asteroid : obstacles) { //spawn terrain
                        if(i == std::get<3>(asteroid)) //If there's a obstacle on rendered tile
                            RenderDevice::Draw(std::get<0>(asteroid), std::get<2>(asteroid));
                    }
                }
            }

            if(seconds == 3){ //3 second countdown up
                ship.automatic = true;
                ship.disableControls = false;
            }
            else if(seconds < 3){ //Counting down
                ship.automatic = false;
                ship.disableControls = true;
            }

            if(ship.Update(dt, tiles)){ //if reset            
                points = 0;
                start = std::chrono::high_resolution_clock::now(); //reset clock
                ship.automatic = false;
                ship.disableControls = true;
                won = false;
                saved = false;
            }
            if(ship.movementIndex >= static_cast<float>(tiles.size())-1){ //if reached end
                won = true;
            }

            //time calculations
            const auto end = std::chrono::high_resolution_clock::now();

            seconds = (int)diff.count();
            if(diff.count() != 0)
            {
                previousTime = static_cast<float>(diff.count());
            }
            diff = end - start;
            frameTime = static_cast<float>(diff.count()) - previousTime;

            if(!won){
                progress = (int)((ship.movementIndex / amountOfTiles) * 100); //Progress in %
                this->collisionsOn = !ship.disableCollisions;
                collided = ship.CheckCollisions();

                if(!ship.disableCollisions)
                { //Collisions and respawning
                    if (collided && renderCar) //Game over
                    {
                        renderCar = false;
                        timer = static_cast<float>(seconds) + 3;
                        ship.disableControls = true;
                        ship.automatic = false;
                        if(!saved)
                            saveScore(points);
                        saved = true;
                    }
                }
                if(static_cast<float>(seconds) >= timer && !renderCar)
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
                    if(ship.automatic) //Change points while podracer is moving
                    {
                        points = (int)( (100 * (diff.count()-3)) / (span));
                    }
                }
            }
            else{
                progress = 100; //100% progress
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

            RenderNanoVG(vg);

            // transfer new frame to window
            this->window->SwapBuffers();

            auto timeEnd = std::chrono::steady_clock::now();
            dt = static_cast<float>(std::min(0.04, std::chrono::duration<double>(timeEnd - timeStart).count()));

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

        nvgFontSize(vg, 16.0f);
        nvgFontFace(vg, "sans");
        nvgFillColor(vg, nvgRGBA(255, 0, 0, 255));


        nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);

        //Text for frames (fps)
        char buf[100];
        sprintf(buf, "Fps: %f", frames);
        nvgText(vg, 550, 10, buf, nullptr);

        //Text for collisions being on/off
        if(collisionsOn)
            nvgText(vg, 60, 460, "Collisions: ON", nullptr);
        else
            nvgText(vg, 60, 460, "Collisions: OFF", nullptr);

        //Text for points gained
        nvgFontSize(vg, 32.0f);
        sprintf(buf, "Points: %i", points);
        nvgText(vg, 320, 20, buf, nullptr);

        //Text for tile progress
        sprintf(buf, "Progress: %i%%", progress);
        nvgText(vg, 480, 440, buf, nullptr);

        //Text for difficulty
        switch (difficulty) {
            case 1:
                sprintf(buf, "Difficulty: Easy");
                break;
            case 2:
                sprintf(buf, "Difficulty: Medium");
                break;
            case 3:
                sprintf(buf, "Difficulty: Hard");
                break;
            default:
                sprintf(buf, "Difficulty: Easy");
        }
        nvgFontSize(vg, 16.0f);
        nvgText(vg, 80, 10, buf, NULL);

        //Text for game over
        if(!renderCar){
            prevPoints = loadScore();
            sprintf(buf, "%s%i", "Previous High Score:", prevPoints);
            nvgFontSize(vg, 50.0f);
            nvgText(vg, 320, 160, "GAME OVER", nullptr);
            nvgText(vg, 320, 220, buf, nullptr);
        }

        //Text for win
        if(won){
            prevPoints = loadScore();
            sprintf(buf, "%s%i", "Previous High Score:", prevPoints);
            nvgFontSize(vg, 50.0f);
            nvgText(vg, 320, 160, "YOU WIN!", nullptr);
            nvgText(vg, 320, 220, buf, nullptr);
        }

        //Text for countdown
        nvgFontSize(vg, 100.0f);
        if(seconds < 3){
            sprintf(buf, "%i", 3-seconds);
            nvgText(vg, 320, 180, buf, nullptr);
        }
        else if(seconds == 3){
            nvgText(vg, 320, 180, "GO!", nullptr);
        }

        nvgRestore(vg);
    }

}