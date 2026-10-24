#pragma once
#include "core/MovementRules.h"
#include "generation/IMapGenerator.h"
#include <random>

// Generates a random map that the algorithms can always solve.
class RandomMapGenerator : public IMapGenerator {
public:
    RandomMapGenerator(const MovementRules& rules, const GenerationSettings& settings, std::mt19937& rng);
    void generate(Grid& grid) override;

private:
    void scatterWalls(Grid& grid);
    void placeDistantEndpoints(Grid& grid);
    void carveCorridor(Grid& grid);

    const MovementRules& rules_;
    const GenerationSettings& settings_;
    std::mt19937& rng_;
};
