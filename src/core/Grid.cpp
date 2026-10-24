#include "core/Grid.h"
#include <stdexcept>
#include <string>

Grid::Grid(int width, int height) { resize(width, height); }

bool Grid::inBounds(int x, int y) const { return x >= 0 && y >= 0 && x < width_ && y < height_; }

bool Grid::isBlocked(int x, int y) const { return !inBounds(x, y) || at(index(x, y)) == CellType::Wall; }

void Grid::set(int cell, CellType type) {
    checkCell(cell);
    cells_[cell] = type;
}

void Grid::fill(CellType type) { cells_.assign(cells_.size(), type); }

void Grid::resize(int width, int height) {
    if (width < kMinSize || height < kMinSize || width > kMaxSize || height > kMaxSize)
        throw std::invalid_argument("Grid size must be within " + std::to_string(kMinSize) + "-" + std::to_string(kMaxSize) +
                                    ": " + std::to_string(width) + "x" + std::to_string(height));
    width_ = width;
    height_ = height;
    cells_.assign(width * height, CellType::Empty);
    placeDefaultEndpoints();
}

void Grid::setStart(int cell) {
    set(cell, CellType::Empty);
    start_ = cell;
}

void Grid::setGoal(int cell) {
    set(cell, CellType::Empty);
    goal_ = cell;
}

void Grid::placeDefaultEndpoints() {
    setStart(index(1, height_ / 2));
    setGoal(index(width_ - 2, height_ / 2));
}

void Grid::checkCell(int cell) const {
    if (cell < 0 || cell >= cellCount())
        throw std::out_of_range("Invalid cell index: " + std::to_string(cell));
}
