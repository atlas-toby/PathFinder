#include "core/Reachability.h"

bool isGoalReachable(const Grid& grid, const MovementRules& rules) {
    std::vector<bool> seen(grid.cellCount(), false);
    std::vector<int> queue{grid.start()};
    seen[grid.start()] = true;
    NeighborList neighbors;
    for (size_t head = 0; head < queue.size(); head++) {
        if (queue[head] == grid.goal()) return true;
        int count = rules.neighbors(queue[head], neighbors);
        for (int k = 0; k < count; k++) {
            int next = neighbors[k].cell;
            if (seen[next]) continue;
            seen[next] = true;
            queue.push_back(next);
        }
    }
    return false;
}
