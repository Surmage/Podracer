//------------------------------------------------------------------------------
// main.cc
// (C) 2015-2018 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "podracerapp.h"
#include <iostream>

int
main(int argc, const char** argv)
{
    std::cout << "Hello World";
    Game::PodRacerApp app;
    if (app.Open())
    {
        app.Run();
        app.Close();
    }
    app.Exit();
	
}