#pragma once
#include "core/CellType.h"
#include <vector>

class Grid {
public:
    static constexpr int kMinSize = 5;
    static constexpr int kMaxSize = 300;

    Grid(int width, int height);

    int width() const { return width_; }
    int height() const { return height_; }
    int cellCount() const { return width_ * height_; }
    int index(int x, int y) const { return y * width_ + x; }
    int xOf(int cell) const { return cell % width_; }
    int yOf(int cell) const { return cell / width_; }
    bool inBounds(int x, int y) const;
    bool isBlocked(int x, int y) const;

    CellType at(int cell) const { return cells_[cell]; }
    void set(int cell, CellType type);
    void fill(CellType type);
    void resize(int width, int height);

    int start() const { return start_; }
    int goal() const { return goal_; }
    void setStart(int cell);
    void setGoal(int cell);
    bool isEndpoint(int cell) const { return cell == start_ || cell == goal_; }
    void placeDefaultEndpoints();

private:
    void checkCell(int cell) const;

    int width_ = 0, height_ = 0;
    std::vector<CellType> cells_;
    int start_ = 0, goal_ = 0;
};
