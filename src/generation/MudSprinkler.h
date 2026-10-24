#pragma once
#include "core/Grid.h"
#include <random>

// Turns the given fraction of empty cells (except start/goal) into mud.
void sprinkleMud(Grid& grid, float density, std::mt19937& rng);
