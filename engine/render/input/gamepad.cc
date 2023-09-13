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
    const float* Gamepad::getAxis() {
        axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
        return axes;
    }
    const unsigned char* Gamepad::getButtons() {
        buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);

        return buttons;
    }
    void Gamepad::setAxis() {
        glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
    }
    void Gamepad::setButtons() {
        glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);
    }
} // namespace Input
