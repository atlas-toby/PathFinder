#pragma once
#include "service/SimulationService.h"
#include "ui/ColorTheme.h"
#include "ui/GridLayout.h"
#include <vector>

// Draws the grid, search state, path and agent with fixed-function OpenGL.
class GridRenderer {
public:
    GridRenderer(const Grid& grid, const SimulationService& simulation, const ColorTheme& colors);
    void draw(const GridLayout& layout, int framebufferWidth, int framebufferHeight, float viewWidth,
              float viewHeight);

private:
    void clearAndProject(int framebufferWidth, int framebufferHeight, float viewWidth, float viewHeight) const;
    void drawCells(const GridLayout& layout);
    void drawAgent(const GridLayout& layout) const;
    void markPathCells();
    Color baseColor(int cell) const;
    Color searchColor(int cell, const Color& base) const;
    Color cellColor(int cell) const;

    const Grid& grid_;
    const SimulationService& simulation_;
    const ColorTheme& colors_;
    std::vector<bool> onPath_;
};
