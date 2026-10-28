#pragma once

#include <glad/glad.h>
#include "../Rendering/Shader.h"
#include <vector>

class FluidSimulation
{
public:
    static constexpr int GridWidth = 256;
    static constexpr int GridHeight = 256;

    enum CellType : unsigned int
    {
        Empty,
        Wall,
        Water,
        Soil,
        LooseSoil
    };

    FluidSimulation();
    ~FluidSimulation();

    FluidSimulation(const FluidSimulation&) = delete;
    FluidSimulation& operator=(const FluidSimulation&) = delete;

    void ApplyBrush(int x0, int y0, int x1, int y1, CellType type, int radius);
    void Update();
    void Render(int width, int height);

private:
    struct Cell
    {
        unsigned int type;
        unsigned int durability;
        unsigned int padding[2];
    };

    Shader movementShader;
    Shader computeShader;
    Shader renderShader;
    Shader postprocessShader;

    GLuint buffers[2] = {};
    GLuint movementBuffer = 0;
    GLuint fullscreenVAO = 0;

    GLuint framebuffer = 0;
    GLuint sceneTexture = 0;
    GLuint waterMaskTexture = 0;

    int renderWidth = 0;
    int renderHeight = 0;

    int readIndex = 0;
    unsigned int frame = 0;

    bool LoadInitialMap(const char* path, std::vector<Cell>& cells);
    void SetCell(int x, int y, CellType type, int radius);
    void DrawLine(int x0, int y0, int x1, int y1, CellType type, int radius);
    void EnsureRenderTargets(int width, int height);
};
