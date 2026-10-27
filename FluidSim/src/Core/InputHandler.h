#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "FluidSimulation.h"

class InputHandler
{
public:
    void HandleInput(GLFWwindow* window, FluidSimulation& simulation);

private:
    int previousX = -1;
    int previousY = -1;
    int previousButton = -1;
    int previousAction = -1;
};
