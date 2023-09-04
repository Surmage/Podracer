#pragma once
//------------------------------------------------------------------------------
/**
    @file

    @copyright
    (C) 2022 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "gp_buttons.h"

namespace Input
{

struct Gamepad
{
    /// contains all buttons that was pressed this frame. This is reset every frame by InputServer
    bool pressed[Button::Code::NumGamepadButtons];

    /// contains all buttons that was released this frame. This is reset every frame by InputServer
    bool released[Button::Code::NumGamepadButtons];

    /// contains all buttons that are currently being held. These are reset as buttons receive the release action by InputServer
    bool held[Button::Code::NumGamepadButtons];
};

} // namespace Input
