#include <iostream>
#include <exception>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Core/FluidSimulation.h"
#include "Core/InputHandler.h"

#ifdef _WIN32
extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
#endif

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW.\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Fluid Sandbox", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL initialized successfully!\n";
    std::cout << "GPU: " << glGetString(GL_RENDERER) << '\n';
    std::cout << "OpenGL: " << glGetString(GL_VERSION) << '\n';
    std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n\n";

    try
    {
        FluidSimulation simulation;
        InputHandler inputHandler;

        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();

            inputHandler.HandleInput(window, simulation);
            simulation.Update();

            int framebufferWidth = 0;
            int framebufferHeight = 0;

            glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

            glClear(GL_COLOR_BUFFER_BIT);
            simulation.Render(framebufferWidth, framebufferHeight);

            glfwSwapBuffers(window);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';

        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
