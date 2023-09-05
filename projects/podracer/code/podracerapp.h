#pragma once
//------------------------------------------------------------------------------
/**
	Space game application

	(C) 20222 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/app.h"
#include "render/window.h"

namespace Game
{
    class PodracerApp : public Core::App {
    public:
        /// constructor
        PodracerApp();
        /// destructor
        ~PodracerApp();

        /// open app
        bool Open();
        /// run app
        void Run();
        /// exit app
        void Exit();

    private:
        int points = 0; //Points gathered based on time and difficulty
        int prevPoints = 0; //Previous high score loaded
        int progress = 0; //Progress on course (current tile / max tiles size)
        float frames = 60; //fps
        bool renderCar = true; //rendering car on
        int seconds = 0; //time that has passed since start
        bool collisionsOn = true; //collisions
        bool won = false; //win screen reached
        bool saved = false; //saved after win or lose
        int difficulty = 1; //difficulty to determine spawn amount

        /// show some ui things
        void RenderUI();
        /// render some nanovg stuff
        void RenderNanoVG(NVGcontext* vg);


        Display::Window* window;


    };
}