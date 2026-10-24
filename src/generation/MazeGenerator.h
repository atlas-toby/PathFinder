#pragma once
#include "generation/IMapGenerator.h"
#include <random>
#include <vector>

// Generates a maze with a recursive backtracker; odd coordinates are corridors.
class MazeGenerator : public IMapGenerator {
public:
    MazeGenerator(const GenerationSettings& settings, std::mt19937& rng);
    void generate(Grid& grid) override;

private:
    void carvePassages(Grid& grid);
    int collectDirections(const Grid& grid, int x, int y, int* out) const;
    void placeEndpoints(Grid& grid);

    const GenerationSettings& settings_;
    std::mt19937& rng_;
};
