#include "FluidSimulation.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

namespace
{
    constexpr int WallRadius = 2;
    constexpr int EraserRadius = 5;
    constexpr int WaterRadius = 2;
    constexpr int SoilRadius = 2;
    constexpr unsigned char MaterialThreshold = 64;
}

FluidSimulation::FluidSimulation()
    : movementShader("src/Shaders/movement.comp"),
    computeShader("src/Shaders/fluid.comp"),
    renderShader("src/Shaders/fullscreen.vert", "src/Shaders/fluid.frag")
{
    std::vector<Cell> cells(GridWidth * GridHeight, { Empty, 0, { 0, 0 } });
    std::vector<unsigned int> destinations(GridWidth * GridHeight);

    LoadInitialMap("src/Assets/initial_map.png", cells);

    for (unsigned int i = 0; i < destinations.size(); i++)
        destinations[i] = i;

    glGenBuffers(2, buffers);

    for (int i = 0; i < 2; i++)
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[i]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, cells.size() * sizeof(Cell), cells.data(), GL_DYNAMIC_DRAW);
    }

    glGenBuffers(1, &movementBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, movementBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, destinations.size() * sizeof(unsigned int), destinations.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenVertexArrays(1, &fullscreenVAO);
}

FluidSimulation::~FluidSimulation()
{
    glDeleteBuffers(2, buffers);
    glDeleteBuffers(1, &movementBuffer);
    glDeleteVertexArrays(1, &fullscreenVAO);
}

bool FluidSimulation::LoadInitialMap(const char* path, std::vector<Cell>& cells)
{
    int imageWidth = 0;
    int imageHeight = 0;
    int channels = 0;

    unsigned char* pixels = stbi_load(path, &imageWidth, &imageHeight, &channels, 4);

    if (!pixels)
    {
        std::cerr << "Failed to load initial map: " << path << " (" << stbi_failure_reason() << ")\n";
        return false;
    }

    for (int y = 0; y < GridHeight; y++)
    {
        for (int x = 0; x < GridWidth; x++)
        {
            int sourceX = x * imageWidth / GridWidth;
            int sourceY = (GridHeight - 1 - y) * imageHeight / GridHeight;
            int pixelIndex = (sourceY * imageWidth + sourceX) * 4;

            unsigned char red = pixels[pixelIndex];
            unsigned char green = pixels[pixelIndex + 1];

            Cell cell = { Empty, 0, { 0, 0 } };

            if (red >= MaterialThreshold)
            {
                cell.type = Wall;
            }
            else if (green >= MaterialThreshold)
            {
                cell.type = Soil;
                cell.durability = 12u;
            }

            cells[y * GridWidth + x] = cell;
        }
    }

    stbi_image_free(pixels);
    return true;
}

void FluidSimulation::SetCell(int x, int y, CellType type, int radius)
{
    Cell cell = 
    {
        static_cast<unsigned int>(type),
        type == Soil ? 12u : 0u,
        { 0, 0 }
    };

    for (int offsetY = -radius; offsetY <= radius; offsetY++)
    {
        for (int offsetX = -radius; offsetX <= radius; offsetX++)
        {
            if (offsetX * offsetX + offsetY * offsetY > radius * radius)
                continue;

            int cellX = x + offsetX;
            int cellY = y + offsetY;

            if (cellX < 0 || cellX >= GridWidth || cellY < 0 || cellY >= GridHeight)
                continue;

            unsigned int index = static_cast<unsigned int>(cellY * GridWidth + cellX);

            if (type == Water || type == Soil)
            {
                Cell existing;

                glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[readIndex]);
                glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, index * sizeof(Cell), sizeof(Cell), &existing);

                if (existing.type != Empty)
                    continue;
            }

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[readIndex]);
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, index * sizeof(Cell), sizeof(Cell), &cell);
        }
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void FluidSimulation::DrawLine(int x0, int y0, int x1, int y1, CellType type, int radius)
{
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int steps = std::max(dx, dy);

    if (steps == 0)
    {
        SetCell(x0, y0, type, radius);
        return;
    }

    for (int i = 0; i <= steps; i++)
    {
        float t = static_cast<float>(i) / static_cast<float>(steps);
        int x = static_cast<int>(x0 + (x1 - x0) * t);
        int y = static_cast<int>(y0 + (y1 - y0) * t);

        SetCell(x, y, type, radius);
    }
}

void FluidSimulation::ApplyBrush(int x0, int y0, int x1, int y1, CellType type,int radius)
{
    DrawLine(x0, y0, x1, y1, type, radius);
}

void FluidSimulation::Update()
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffers[readIndex]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, movementBuffer);

    movementShader.Use();
    movementShader.SetInt("gridWidth", GridWidth);
    movementShader.SetInt("gridHeight", GridHeight);
    movementShader.SetInt("frame", static_cast<int>(frame));

    glDispatchCompute((GridWidth + 15) / 16, (GridHeight + 15) / 16, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffers[readIndex]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, movementBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, buffers[1 - readIndex]);

    computeShader.Use();
    computeShader.SetInt("gridWidth", GridWidth);
    computeShader.SetInt("gridHeight", GridHeight);
    computeShader.SetInt("frame", static_cast<int>(frame));

    glDispatchCompute((GridWidth + 15) / 16, (GridHeight + 15) / 16, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);

    readIndex = 1 - readIndex;
    frame++;

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, 0);
}

void FluidSimulation::Render(int width, int height)
{
    if (width <= 0 || height <= 0)
        return;

    glViewport(0, 0, width, height);

    renderShader.Use();
    renderShader.SetVec2("gridSize", static_cast<float>(GridWidth), static_cast<float>(GridHeight));

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffers[readIndex]);

    glBindVertexArray(fullscreenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
}
