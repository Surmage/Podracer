#pragma once
//------------------------------------------------------------------------------
/**
    @file

    @copyright
    (C) 2022 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------

namespace Input
{

struct Gamepad
{


    const float* axes;
    const unsigned char* buttons;
    int axesCount;
    int count;

    const float* getAxis();
    const unsigned char* getButtons();
    void setAxis();
    void setButtons();
};

} // namespace Input
