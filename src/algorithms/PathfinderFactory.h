#pragma once
#include "algorithms/AlgorithmType.h"
#include "algorithms/IPathfinder.h"
#include "core/Grid.h"
#include "core/Heuristic.h"
#include "core/MovementRules.h"
#include <memory>
#include <random>

class PathfinderFactory {
public:
    PathfinderFactory(const Grid& grid, const MovementRules& rules, std::mt19937& rng);

    std::unique_ptr<IPathfinder> create(AlgorithmType algorithm, HeuristicType heuristic) const;

private:
    std::unique_ptr<IPathfinder> createBestFirst(AlgorithmType algorithm, HeuristicType heuristic) const;

    const Grid& grid_;
    const MovementRules& rules_;
    std::mt19937& rng_;
};
