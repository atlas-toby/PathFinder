#include "algorithms/BfsPathfinder.h"

BfsPathfinder::BfsPathfinder(const Grid& grid, const MovementRules& rules)
    : grid_(grid), rules_(rules), state_(grid.cellCount(), grid.start()), queue_{grid.start()} {}

void BfsPathfinder::step() {
    if (state_.isDone()) return;
    if (queue_.empty()) return state_.finish(false);
    int cell = queue_.front();
    queue_.pop_front();
    state_.close(cell);
    if (cell == grid_.goal()) return state_.finish(true);
    expand(cell);
}

void BfsPathfinder::expand(int cell) {
    NeighborList neighbors;
    int count = rules_.neighbors(cell, neighbors);
    for (int k = 0; k < count; k++) {
        int next = neighbors[k].cell;
        if (!state_.isUnseen(next)) continue;
        state_.open(next, cell, state_.costTo(cell) + neighbors[k].cost);
        queue_.push_back(next);
    }
}
