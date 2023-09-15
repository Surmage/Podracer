//------------------------------------------------------------------------------
//  @file gamepad.cc
//  @copyright (C) 2022 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "gamepad.h"
#include "render/window.h"
#include <iostream>

namespace Input
{
    int Gamepad::getJoy(){
        for (int i = 0; i < 15; i++)
        {
           if (glfwJoystickPresent(i) && glfwGetJoystickName(i) == std::string("Logitech Gamepad F310") ||
                glfwJoystickPresent(i) && glfwGetJoystickName(i) == std::string("Microsoft X-Box One S pad") ||
                glfwJoystickPresent(i) && glfwGetJoystickName(i) == std::string("Sony Interactive Entertainment Wireless Controller"))
           {
               return i;
               id = i;
           }
        }
        return -1;
    }
    const float* Gamepad::getAxis() {
        axes = glfwGetJoystickAxes(id, &axesCount);
        return axes;
    }
    const unsigned char* Gamepad::getButtons() {
        buttons = glfwGetJoystickButtons(id, &count);
        return buttons;
    }
    void Gamepad::setAxis() {
        glfwGetJoystickAxes(id, &axesCount);
    }
    void Gamepad::setButtons() {
        glfwGetJoystickButtons(id, &count);
    }
} // namespace Input
