#include "core/Heuristic.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>

float estimateDistance(HeuristicType type, const Grid& grid, int from, int to) {
    float dx = float(std::abs(grid.xOf(from) - grid.xOf(to)));
    float dy = float(std::abs(grid.yOf(from) - grid.yOf(to)));
    switch (type) {
        case HeuristicType::Euclidean: return std::sqrt(dx * dx + dy * dy);
        case HeuristicType::Chebyshev: return std::max(dx, dy);
        default: return dx + dy;
    }
}
