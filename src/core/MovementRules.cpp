#include "core/MovementRules.h"

namespace {
constexpr float kStraightCost = 1.f;
constexpr float kDiagonalCost = 1.41421356f;
constexpr int kStraightDirections = 4;
constexpr int kDx[kMaxNeighbors] = {1, -1, 0, 0, 1, 1, -1, -1};
constexpr int kDy[kMaxNeighbors] = {0, 0, 1, -1, 1, -1, 1, -1};
}  // namespace

MovementRules::MovementRules(const Grid& grid, const MovementSettings& settings)
    : grid_(grid), settings_(settings) {}

int MovementRules::neighbors(int cell, NeighborList& out) const {
    int x = grid_.xOf(cell), y = grid_.yOf(cell), count = 0;
    int directions = settings_.allowDiagonal ? kMaxNeighbors : kStraightDirections;
    for (int d = 0; d < directions; d++) {
        int nx = x + kDx[d], ny = y + kDy[d];
        if (grid_.isBlocked(nx, ny)) continue;
        bool diagonal = d >= kStraightDirections;
        if (diagonal && !isDiagonalAllowed(x, y, kDx[d], kDy[d])) continue;
        int next = grid_.index(nx, ny);
        out[count++] = {next, (diagonal ? kDiagonalCost : kStraightCost) * cellCost(next)};
    }
    return count;
}

float MovementRules::cellCost(int cell) const {
    return grid_.at(cell) == CellType::Mud ? settings_.mudCost : kStraightCost;
}

float MovementRules::stepCost(int from, int to) const {
    bool diagonal = grid_.xOf(from) != grid_.xOf(to) && grid_.yOf(from) != grid_.yOf(to);
    return (diagonal ? kDiagonalCost : kStraightCost) * cellCost(to);
}

float MovementRules::pathCost(const std::vector<int>& path) const {
    float total = 0.f;
    for (size_t k = 1; k < path.size(); k++) total += stepCost(path[k - 1], path[k]);
    return total;
}

bool MovementRules::isDiagonalAllowed(int x, int y, int dx, int dy) const {
    if (!settings_.preventCornerCutting) return true;
    return !grid_.isBlocked(x + dx, y) && !grid_.isBlocked(x, y + dy);
}
