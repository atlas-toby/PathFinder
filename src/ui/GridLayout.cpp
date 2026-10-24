#include "ui/GridLayout.h"
#include <algorithm>
#include <cmath>

GridLayout GridLayout::fit(const Grid& grid, float areaWidth, float areaHeight) {
    GridLayout layout;
    layout.cellSize = std::min(areaWidth / grid.width(), areaHeight / grid.height());
    layout.offsetX = (areaWidth - layout.cellSize * grid.width()) * 0.5f;
    layout.offsetY = (areaHeight - layout.cellSize * grid.height()) * 0.5f;
    return layout;
}

std::optional<int> GridLayout::cellAt(const Grid& grid, float pixelX, float pixelY) const {
    int x = int(std::floor((pixelX - offsetX) / cellSize));
    int y = int(std::floor((pixelY - offsetY) / cellSize));
    if (!grid.inBounds(x, y)) return std::nullopt;
    return grid.index(x, y);
}
