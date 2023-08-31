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
        int points = 0;
        int prevPoints = 0;
        float frames = 60;
        bool renderCar = true;
        int seconds = 0;
        bool collisionsOn = true;
        bool won = false;
        bool saved = false;
        int difficulty = 1;
        /// show some ui things
        void RenderUI();
        /// render some nanovg stuff
        void RenderNanoVG(NVGcontext* vg);


        Display::Window* window;


    };
}