#pragma once


namespace Input
{

    struct Button
    {

        enum Code
        {
            A_Button,
            B_Button,
            X_Button,
            Y_Button,
            Left_Bumper,
            Right_Bumper,
            Back_Button,
            Start_Button,
            Guide_Button,
            Left_Thumb,
            Right_Thumb,
            D_Pad_Up,
            D_Pad_Right,
            D_Pad_Down,
            D_Pad_Left,

            NumGamepadButtons,
        };
        static Button::Code::NumGamepadButtons FromGLFW(int32 button);
    };
} // namespace Input