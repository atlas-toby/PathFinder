#include "algorithms/BestFirstPathfinder.h"

namespace {
constexpr int kNone = -1;
}

BestFirstPathfinder::BestFirstPathfinder(const Grid& grid, const MovementRules& rules, PriorityFormula priority,
                                         bool updateImprovedCosts)
    : grid_(grid), rules_(rules), priority_(std::move(priority)), updateImprovedCosts_(updateImprovedCosts),
      state_(grid.cellCount(), grid.start()) {
    queue_.push({priority_(grid.start(), 0.f), grid.start()});
}

void BestFirstPathfinder::step() {
    if (state_.isDone()) return;
    int cell = popNextOpenCell();
    if (cell == kNone) return state_.finish(false);
    state_.close(cell);
    if (cell == grid_.goal()) return state_.finish(true);
    expand(cell);
}

int BestFirstPathfinder::popNextOpenCell() {
    while (!queue_.empty()) {
        int cell = queue_.top().second;
        queue_.pop();
        if (!state_.isClosed(cell)) return cell;
    }
    return kNone;
}

void BestFirstPathfinder::expand(int cell) {
    NeighborList neighbors;
    int count = rules_.neighbors(cell, neighbors);
    for (int k = 0; k < count; k++) {
        int next = neighbors[k].cell;
        float newCost = state_.costTo(cell) + neighbors[k].cost;
        if (shouldSkip(next, newCost)) continue;
        state_.open(next, cell, newCost);
        queue_.push({priority_(next, newCost), next});
    }
}

bool BestFirstPathfinder::shouldSkip(int cell, float newCost) const {
    if (state_.isClosed(cell)) return true;
    if (!updateImprovedCosts_) return state_.hasCost(cell);
    return newCost >= state_.costTo(cell);
}
