#include "generation/MudSprinkler.h"

void sprinkleMud(Grid& grid, float density, std::mt19937& rng) {
    std::uniform_real_distribution<float> chance(0.f, 1.f);
    for (int cell = 0; cell < grid.cellCount(); cell++) {
        if (grid.at(cell) != CellType::Empty || grid.isEndpoint(cell)) continue;
        if (chance(rng) < density) grid.set(cell, CellType::Mud);
    }
}
