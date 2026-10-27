#include "InputHandler.h"

#include <algorithm>

namespace
{
    constexpr int WallRadius = 4;
    constexpr int EraserRadius = 8;
    constexpr int WaterRadius = 2;
    constexpr int SoilRadius = 4;
}

void InputHandler::HandleInput(GLFWwindow* window, FluidSimulation& simulation)
{
#pragma region MousePos
    double mouseX = 0.0;
    double mouseY = 0.0;

    glfwGetCursorPos(window, &mouseX, &mouseY);
#pragma endregion

#pragma region Coords
    int windowWidth = 0;
    int windowHeight = 0;

    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    if (windowWidth <= 0 || windowHeight <= 0)
        return;

    int x = std::clamp(static_cast<int>(mouseX * FluidSimulation::GridWidth / windowWidth), 0, FluidSimulation::GridWidth - 1);
    int y = std::clamp(FluidSimulation::GridHeight - 1 - static_cast<int>(mouseY * FluidSimulation::GridHeight / windowHeight), 0, FluidSimulation::GridHeight - 1);
#pragma endregion

#pragma region Buttons
    bool shiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;

    int button = -1;
    int action = -1;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        button = GLFW_MOUSE_BUTTON_LEFT;
        action = shiftPressed ? 0 : 1;
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        button = GLFW_MOUSE_BUTTON_RIGHT;
        action = 2;
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
    {
        button = GLFW_MOUSE_BUTTON_MIDDLE;
        action = 3;
    }

    if (button == -1)
    {
        previousX = -1;
        previousY = -1;
        previousButton = -1;
        previousAction = -1;
        return;
    }
#pragma endregion

#pragma region Actions
    FluidSimulation::CellType type =
        action == 1 ? FluidSimulation::Wall :
        action == 0 ? FluidSimulation::Empty :
        action == 2 ? FluidSimulation::Water :
        FluidSimulation::Soil;

    int radius =
        action == 1 ? WallRadius :
        action == 0 ? EraserRadius :
        action == 2 ? WaterRadius :
        SoilRadius;

    if (button == previousButton && action == previousAction && previousX >= 0 && previousY >= 0)
    {
        simulation.ApplyBrush(previousX, previousY, x, y, type, radius);
    }
    else
    {
        simulation.ApplyBrush(x, y, x, y, type, radius);
    }

    previousX = x;
    previousY = y;
    previousButton = button;
    previousAction = action;
#pragma endregion

}
