#pragma once
#include "core/Grid.h"
#include "core/MovementRules.h"
#include <optional>
#include <vector>

struct CellPoint {
    float x;  // in cell units, cell center = integer + 0.5
    float y;
};

// Agent that walks along the found path; slows down in mud.
class Agent {
public:
    void restart() { progress_ = 0.f; }
    void advance(float dt, float cellsPerSecond, const std::vector<int>& path, const MovementRules& rules);
    std::optional<CellPoint> position(const std::vector<int>& path, const Grid& grid) const;

private:
    int currentSegment(const std::vector<int>& path) const;

    float progress_ = 0.f;  // position along the path, in path-index units
};
