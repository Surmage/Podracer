#pragma once
//------------------------------------------------------------------------------
/**
    @file

    @copyright
    (C) 2022 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <GLFW/glfw3.h>
#include <string>

namespace Input
{

struct Gamepad
{
    enum Button
    {
        LeftButton = 0,
        RightButton = 1,
        MiddleButton = 2,

        NumGamepadButtons,
    };

    /// contains all buttons that was pressed this frame. This is reset every frame by InputServer
    bool pressed[Button::NumGamepadButtons];

    /// contains all buttons that was released this frame. This is reset every frame by InputServer
    bool released[Button::NumGamepadButtons];

    /// contains all buttons that are currently being held. These are reset as buttons receive the release action by InputServer
    bool held[Button::NumGamepadButtons];

    static void gamepad();
};

} // namespace Input
