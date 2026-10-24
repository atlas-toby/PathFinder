#pragma once
#include "algorithms/IPathfinder.h"
#include "core/Grid.h"
#include "core/MovementRules.h"
#include <random>
#include <vector>

// Moves in random directions; backtracks out of dead ends and never enters them again.
class RandomWalkPathfinder : public IPathfinder {
public:
    RandomWalkPathfinder(const Grid& grid, const MovementRules& rules, std::mt19937& rng);

    void step() override;
    const SearchState& state() const override { return state_; }

private:
    int countUnseenNeighbors(int cell, NeighborList& out) const;

    const Grid& grid_;
    const MovementRules& rules_;
    std::mt19937& rng_;
    SearchState state_;
    std::vector<int> trail_;
};
