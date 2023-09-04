//
// Created by petter on 2023-08-31.
//

#include "gp_buttons.h"
#include "config.h"
#include <GLFW/glfw3.h>
//#include <string>

namespace Input
{
    int gamepad = 0;
    int buttonCount;
    const unsigned char *buttons = glfwGetJoystickButtons(gamepad, &buttonCount);

    Button::Code
    Button::FromGLFW()
    {

        if (GLFW_PRESS == buttons[0])
            return Code::A_Button;
        if (GLFW_PRESS == buttons[1])
            return Code::B_Button;
        if (GLFW_PRESS == buttons[2])
            return Code::X_Button;
        if (GLFW_PRESS == buttons[3])
            return Code::Y_Button;
        if (GLFW_PRESS == buttons[4])
            return Code::Left_Bumper;
        if (GLFW_PRESS == buttons[5])
            return Code::Right_Bumper;
        if (GLFW_PRESS == buttons[6])
            return Code::Back_Button;
        if (GLFW_PRESS == buttons[7])
            return Code::Start_Button;
        if (GLFW_PRESS == buttons[8])
            return Code::Guide_Button;
        if (GLFW_PRESS == buttons[9])
            return Code::Left_Thumb;
        if (GLFW_PRESS == buttons[10])
            return Code::Right_Thumb;
        if (GLFW_PRESS == buttons[11])
            return Code::D_Pad_Up;
        if (GLFW_PRESS == buttons[12])
            return Code::D_Pad_Right;
        if (GLFW_PRESS == buttons[13])
            return Code::D_Pad_Down;
        if (GLFW_PRESS == buttons[14])
            return Code::D_Pad_Left;
    }
} // namespace Input

