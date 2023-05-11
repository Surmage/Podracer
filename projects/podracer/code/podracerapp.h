#pragma once

#include "core/app.h"
#include "render/window.h"

namespace Game
{
    class PodRacerApp : public Core::App
    {
    public:
        /// constructor
        PodRacerApp();
        /// destructor
        ~PodRacerApp();

        /// open app
        bool Open();
        /// run app
        void Run();
        /// exit app
        void Exit();
    private:

        /// show some ui things
        void RenderUI();
        /// render some nanovg stuff
        void RenderNanoVG(NVGcontext* vg);

        Display::Window* window;
    };
} // namespace Game