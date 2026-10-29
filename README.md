# FluidSimulation
My goal was to build a 2D material simulation from scratch using C++ and OpenGL, without relying on a game engine. Using compute shaders, I implemented a grid-based simulation where materials can move and interact with each other. The simulation uses a double buffer grid and a two-pass update process to handle movement and resolve conflicts between cells. The project currently features water, walls, and soil, including interactions erosion. 

## Dependencies
- C++ 20
- OpenGL 4.6
- [GLAD](https://github.com/dav1dde/glad)
- [GLFW](https://www.glfw.org/)
