////------------------------------------------------------------------------------
////  @file gamepad.cc
////  @copyright (C) 2022 Individual contributors, see AUTHORS file
////------------------------------------------------------------------------------
//#include "config.h"
//#include "gamepad.h"
//
//namespace Input
//{
//    void Gamepad::gamepad()
//    {
//        int gamepad = 0;
//        for (int i = 0; i < 15; i++)
//        {
//            if (glfwJoystickPresent(i) && glfwGetJoystickName(i) == std::string("Logitech Gamepad F310") ||
//                glfwJoystickPresent(i) && glfwGetJoystickName(i) == std::string("Microsoft X-Box One S pad") ||
//                glfwJoystickPresent(i) && glfwGetJoystickName(i) == std::string("Sony Interactive Entertainment Wireless Controller"))
//                gamepad = i;
//        }
//
//        if (glfwGetJoystickName(gamepad) == std::string("Logitech Gamepad F310") ||
//            glfwGetJoystickName(gamepad) == std::string("Microsoft X-Box One S pad") ||
//            glfwGetJoystickName(gamepad) == std::string("Sony Interactive Entertainment Wireless Controller") )
//        {
//
//            //----------CONTROLLER NAME CONFIGURATION-------------
//            printf("Joystick %d name: %s\n", gamepad, glfwGetJoystickName(gamepad));
//            //----------END OF CONTROLLER NAME CONFIGURATION-------------
//
//            //----------CONTROLLER AXIS CONFIGURATION-------------
//            int axesCount;
//            const float *axes = glfwGetJoystickAxes(gamepad, &axesCount);
//            printf("Number of axes available: %d\n", axesCount);
//
//            // ps5, xbox & logitech (ltu) controller axis configuration
//            printf("Left Stick X Axis:  %f\n", axes[0]);
//            printf("Left Stick Y Axis:  %f\n", axes[1]);
//            printf("Left Trigger/L2:    %f\n", axes[2]);
//            printf("Right Stick X Axis: %f\n", axes[3]);
//            printf("Right Stick Y Axis: %f\n", axes[4]);
//            printf("Right Trigger/R2:   %f\n", axes[5]);
//            //----------END OF CONTROLLER AXIS CONFIGURATION-------------
//
//            //----------CONTROLLER BUTTON CONFIGURATION-------------
//            int buttonCount;
//            const unsigned char *buttons = glfwGetJoystickButtons(gamepad, &buttonCount);
//            printf("Number of buttons available: %d\n", buttonCount);
//
//            if (std::string(glfwGetJoystickName(gamepad)) == "Sony Interactive Entertainment Wireless Controller") {
//                printf("PS5 controller detected\n");
//                if (GLFW_PRESS == buttons[0])
//                    printf("X button pressed\n");
//                if (GLFW_PRESS == buttons[1])
//                    printf("Circle button pressed\n");
//                if (GLFW_PRESS == buttons[2])
//                    printf("Triangle button pressed\n");
//                if (GLFW_PRESS == buttons[3])
//                    printf("Square button pressed\n");
//                if (GLFW_PRESS == buttons[4])
//                    printf("L1 button pressed\n");
//                if (GLFW_PRESS == buttons[5])
//                    printf("R1 button pressed\n");
//                if (GLFW_PRESS == buttons[6])
//                    printf("L2 button pressed\n");
//                if (GLFW_PRESS == buttons[7])
//                    printf("R2 button pressed\n");
//                if (GLFW_PRESS == buttons[8])
//                    printf("Share button pressed\n");
//                if (GLFW_PRESS == buttons[9])
//                    printf("Options button pressed\n");
//                if (GLFW_PRESS == buttons[10])
//                    printf("PS button pressed\n");
//                if (GLFW_PRESS == buttons[11])
//                    printf("L3 button pressed\n");
//                if (GLFW_PRESS == buttons[12])
//                    printf("R3 button pressed\n");
//                if (GLFW_PRESS == buttons[13])
//                    printf("D-pad UP button pressed\n");
//                if (GLFW_PRESS == buttons[14])
//                    printf("D-pad RIGHT button pressed\n");
//                if (GLFW_PRESS == buttons[15])
//                    printf("D-pad DOWN button pressed\n");
//                if (GLFW_PRESS == buttons[16])
//                    printf("D-pad LEFT button pressed\n");
//            } else if (std::string(glfwGetJoystickName(gamepad)) == "Logitech Gamepad F310"
//                       || std::string(glfwGetJoystickName(gamepad)) == "Microsoft X-Box One S pad") {
//                printf("Logitech or Xbox controller detected\n");
//                if (GLFW_PRESS == buttons[0])
//                     //printf("A button pressed\n");
//
//                if (GLFW_PRESS == buttons[1])
//                    printf("B button pressed\n");
//                if (GLFW_PRESS == buttons[2])
//                    printf("X button pressed\n");
//                if (GLFW_PRESS == buttons[3])
//                    printf("Y button pressed\n");
//                if (GLFW_PRESS == buttons[4])
//                    printf("LB button pressed\n");
//                if (GLFW_PRESS == buttons[5])
//                    printf("RB button pressed\n");
//                if (GLFW_PRESS == buttons[6])
//                    printf("Back button pressed\n");
//                if (GLFW_PRESS == buttons[7])
//                    printf("Start button pressed\n");
//                if (GLFW_PRESS == buttons[8])
//                    printf("Logitech/Xbox button pressed\n");
//                if (GLFW_PRESS == buttons[9])
//                    printf("L3 / Left Stick Click pressed\n");
//                if (GLFW_PRESS == buttons[10])
//                    printf("R3 / Right Stick Click pressed\n");
//                if (GLFW_PRESS == buttons[11])
//                    printf("D-pad UP button pressed\n");
//                if (GLFW_PRESS == buttons[12])
//                    printf("D-pad RIGHT button pressed\n");
//                if (GLFW_PRESS == buttons[13])
//                    printf("D-pad DOWN button pressed\n");
//                if (GLFW_PRESS == buttons[14])
//                    printf("D-pad LEFT button pressed\n");
//            } else {
//                printf("Unknown controller detected\n");
//                for (int i = 0; i < buttonCount; i++) {
//                    if (GLFW_PRESS == buttons[i])
//                        printf("Button %d pressed\n", i);
//                }
//            }
//        }
//
//    }
//} // namespace Input
