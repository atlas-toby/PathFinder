#pragma once
#include "core/Grid.h"
#include <optional>

// On-screen placement of the grid: cell size and centered top-left corner (pixels).
struct GridLayout {
    float cellSize = 1.f;
    float offsetX = 0.f;
    float offsetY = 0.f;

    static GridLayout fit(const Grid& grid, float areaWidth, float areaHeight);
    std::optional<int> cellAt(const Grid& grid, float pixelX, float pixelY) const;
    float toPixelX(float cellX) const { return offsetX + cellX * cellSize; }
    float toPixelY(float cellY) const { return offsetY + cellY * cellSize; }
};
