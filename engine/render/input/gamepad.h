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
        A_Button = 0,
        B_Button = 1,
        X_Button = 2,
        Y_Button = 3,
        Left_Bumper = 4,
        Right_Bumper = 5,
        Back_Button = 6,
        Start_Button = 7,
        Guide_Button = 8,
        Left_Thumb = 9,
        Right_Thumb = 10,
        D_Pad_Up = 11,
        D_Pad_Right = 12,
        D_Pad_Down = 13,
        D_Pad_Left = 14,

        NumGamepadButtons,
    };

    /// contains all buttons that was pressed this frame. This is reset every frame by InputServer
    bool pressed[Button::NumGamepadButtons];

    /// contains all buttons that was released this frame. This is reset every frame by InputServer
    bool released[Button::NumGamepadButtons];

    /// contains all buttons that are currently being held. These are reset as buttons receive the release action by InputServer
    bool held[Button::NumGamepadButtons];

    //static void gamepad();
};

} // namespace Input
