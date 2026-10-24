#include "algorithms/PathfinderFactory.h"
#include "algorithms/BestFirstPathfinder.h"
#include "algorithms/BfsPathfinder.h"
#include "algorithms/QuantumWalkPathfinder.h"
#include "algorithms/RandomWalkPathfinder.h"

PathfinderFactory::PathfinderFactory(const Grid& grid, const MovementRules& rules, std::mt19937& rng)
    : grid_(grid), rules_(rules), rng_(rng) {}

std::unique_ptr<IPathfinder> PathfinderFactory::create(AlgorithmType algorithm, HeuristicType heuristic) const {
    switch (algorithm) {
        case AlgorithmType::Bfs: return std::make_unique<BfsPathfinder>(grid_, rules_);
        case AlgorithmType::RandomWalk: return std::make_unique<RandomWalkPathfinder>(grid_, rules_, rng_);
        case AlgorithmType::QuantumWalk: return std::make_unique<QuantumWalkPathfinder>(grid_, rules_);
        default: return createBestFirst(algorithm, heuristic);
    }
}

std::unique_ptr<IPathfinder> PathfinderFactory::createBestFirst(AlgorithmType algorithm,
                                                                HeuristicType heuristic) const {
    const Grid& grid = grid_;
    auto toGoal = [&grid, heuristic](int cell) { return estimateDistance(heuristic, grid, cell, grid.goal()); };
    switch (algorithm) {
        case AlgorithmType::AStar:
            return std::make_unique<BestFirstPathfinder>(
                grid_, rules_, [toGoal](int cell, float cost) { return cost + toGoal(cell); }, true);
        case AlgorithmType::Greedy:
            return std::make_unique<BestFirstPathfinder>(
                grid_, rules_, [toGoal](int cell, float) { return toGoal(cell); }, false);
        default:
            return std::make_unique<BestFirstPathfinder>(
                grid_, rules_, [](int, float cost) { return cost; }, true);
    }
}
