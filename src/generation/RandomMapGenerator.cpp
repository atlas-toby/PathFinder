#include "generation/RandomMapGenerator.h"
#include "core/Reachability.h"
#include "generation/MudSprinkler.h"
#include <cstdlib>

namespace {
constexpr int kMaxAttempts = 300;
constexpr int kMaxEndpointTries = 200;
}  // namespace

RandomMapGenerator::RandomMapGenerator(const MovementRules& rules, const GenerationSettings& settings,
                                       std::mt19937& rng)
    : rules_(rules), settings_(settings), rng_(rng) {}

void RandomMapGenerator::generate(Grid& grid) {
    bool solvable = false;
    for (int attempt = 0; attempt < kMaxAttempts && !solvable; attempt++) {
        scatterWalls(grid);
        placeDistantEndpoints(grid);
        solvable = isGoalReachable(grid, rules_);
    }
    if (!solvable) carveCorridor(grid);
    sprinkleMud(grid, settings_.mudDensity, rng_);
}

void RandomMapGenerator::scatterWalls(Grid& grid) {
    std::uniform_real_distribution<float> chance(0.f, 1.f);
    for (int cell = 0; cell < grid.cellCount(); cell++)
        grid.set(cell, chance(rng_) < settings_.wallDensity ? CellType::Wall : CellType::Empty);
}

void RandomMapGenerator::placeDistantEndpoints(Grid& grid) {
    std::uniform_int_distribution<int> anyCell(0, grid.cellCount() - 1);
    int minDistance = (grid.width() + grid.height()) / 2;
    int start = 0, goal = 0;
    for (int tries = 0; tries < kMaxEndpointTries; tries++) {
        start = anyCell(rng_);
        goal = anyCell(rng_);
        int distance = std::abs(grid.xOf(start) - grid.xOf(goal)) + std::abs(grid.yOf(start) - grid.yOf(goal));
        if (distance >= minDistance) break;
    }
    grid.setStart(start);
    grid.setGoal(goal);
}

// If the wall density is too high, carves an L-shaped corridor from start to goal.
void RandomMapGenerator::carveCorridor(Grid& grid) {
    int x = grid.xOf(grid.start()), y = grid.yOf(grid.start());
    int goalX = grid.xOf(grid.goal()), goalY = grid.yOf(grid.goal());
    for (; x != goalX; x += x < goalX ? 1 : -1) grid.set(grid.index(x, y), CellType::Empty);
    for (; y != goalY; y += y < goalY ? 1 : -1) grid.set(grid.index(x, y), CellType::Empty);
}
