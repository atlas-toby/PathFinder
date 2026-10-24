#pragma once
#include "algorithms/IPathfinder.h"
#include "algorithms/MinQueue.h"
#include "core/Grid.h"
#include "core/MovementRules.h"
#include <functional>

// A*, Dijkstra and Greedy differ only in their priority formula and cost-improvement policy.
using PriorityFormula = std::function<float(int cell, float costSoFar)>;

class BestFirstPathfinder : public IPathfinder {
public:
    BestFirstPathfinder(const Grid& grid, const MovementRules& rules, PriorityFormula priority,
                        bool updateImprovedCosts);

    void step() override;
    const SearchState& state() const override { return state_; }

private:
    int popNextOpenCell();
    void expand(int cell);
    bool shouldSkip(int cell, float newCost) const;

    const Grid& grid_;
    const MovementRules& rules_;
    PriorityFormula priority_;
    bool updateImprovedCosts_;
    SearchState state_;
    MinQueue queue_;
};
