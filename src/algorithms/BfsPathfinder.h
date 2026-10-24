#pragma once
#include "algorithms/IPathfinder.h"
#include "core/Grid.h"
#include "core/MovementRules.h"
#include <deque>

// Ignores weights; finds the path with the fewest steps.
class BfsPathfinder : public IPathfinder {
public:
    BfsPathfinder(const Grid& grid, const MovementRules& rules);

    void step() override;
    const SearchState& state() const override { return state_; }

private:
    void expand(int cell);

    const Grid& grid_;
    const MovementRules& rules_;
    SearchState state_;
    std::deque<int> queue_;
};
