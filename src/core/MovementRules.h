#pragma once
#include "core/Grid.h"
#include <array>
#include <vector>

struct MovementSettings {
    bool allowDiagonal = false;
    bool preventCornerCutting = true;
    float mudCost = 5.f;
};

struct Neighbor {
    int cell;
    float cost;
};

static constexpr int kMaxNeighbors = 8;
using NeighborList = std::array<Neighbor, kMaxNeighbors>;

// Decides which cells can be entered and what each move costs.
class MovementRules {
public:
    MovementRules(const Grid& grid, const MovementSettings& settings);

    int neighbors(int cell, NeighborList& out) const;
    float cellCost(int cell) const;
    float stepCost(int from, int to) const;
    float pathCost(const std::vector<int>& path) const;

private:
    bool isDiagonalAllowed(int x, int y, int dx, int dy) const;

    const Grid& grid_;
    const MovementSettings& settings_;
};
