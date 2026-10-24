#include "service/Agent.h"
#include <algorithm>

namespace {
constexpr float kCellCenter = 0.5f;
}

void Agent::advance(float dt, float cellsPerSecond, const std::vector<int>& path, const MovementRules& rules) {
    if (path.size() < 2) return;
    int segment = currentSegment(path);
    float speed = cellsPerSecond / rules.cellCost(path[segment + 1]);
    progress_ = std::min(progress_ + dt * speed, float(path.size() - 1));
}

std::optional<CellPoint> Agent::position(const std::vector<int>& path, const Grid& grid) const {
    if (path.size() < 2) return std::nullopt;
    int segment = currentSegment(path);
    float t = progress_ - segment;
    int from = path[segment], to = path[segment + 1];
    float x = grid.xOf(from) + (grid.xOf(to) - grid.xOf(from)) * t + kCellCenter;
    float y = grid.yOf(from) + (grid.yOf(to) - grid.yOf(from)) * t + kCellCenter;
    return CellPoint{x, y};
}

int Agent::currentSegment(const std::vector<int>& path) const {
    return std::min(int(progress_), int(path.size()) - 2);
}
