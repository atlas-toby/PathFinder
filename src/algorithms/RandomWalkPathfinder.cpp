#include "algorithms/RandomWalkPathfinder.h"

RandomWalkPathfinder::RandomWalkPathfinder(const Grid& grid, const MovementRules& rules, std::mt19937& rng)
    : grid_(grid), rules_(rules), rng_(rng), state_(grid.cellCount(), grid.start()), trail_{grid.start()} {}

void RandomWalkPathfinder::step() {
    if (state_.isDone()) return;
    if (trail_.empty()) return state_.finish(false);
    int cell = trail_.back();
    state_.close(cell);
    if (cell == grid_.goal()) return state_.finish(true);

    NeighborList options;
    int count = countUnseenNeighbors(cell, options);
    if (count == 0) {
        state_.markDead(cell);
        trail_.pop_back();
        return;
    }
    const Neighbor& next = options[std::uniform_int_distribution<int>(0, count - 1)(rng_)];
    state_.open(next.cell, cell, state_.costTo(cell) + next.cost);
    trail_.push_back(next.cell);
}

int RandomWalkPathfinder::countUnseenNeighbors(int cell, NeighborList& out) const {
    NeighborList all;
    int total = rules_.neighbors(cell, all), count = 0;
    for (int k = 0; k < total; k++)
        if (state_.isUnseen(all[k].cell)) out[count++] = all[k];
    return count;
}
