# FluidSimulation
The goal was to build a grid based simulation. I already did this in the past in Unity, so I had the base logic on how to make this work (double buffer, compute shader for the movement etc.).
So this was more of a test to see if I was able to recreate all that without a game engine.

https://github.com/user-attachments/assets/59f62eb5-1237-436c-8022-aaeced9ab7e8

>Note: Inside [postprocess.frag](https://github.com/NRexha/FluidSimulation/blob/ea2d3b19335d1f40ef74082b0a91e258ae0c6b15/FluidSim/src/Shaders/postprocess.frag) I do a blur pass to make a sort of continous flow for the water. I basically just did this for video recording since it looked ugly without it.
>On my monitor, it actually looks really good without this postprocess. If you want to disable it, just replace the last line with: `FragColor = scene;`

## Dependencies
- C++ 20
- OpenGL 4.6
- [GLAD](https://github.com/dav1dde/glad)
- [GLFW](https://www.glfw.org/)
