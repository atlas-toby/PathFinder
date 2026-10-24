#pragma once
#include "core/Grid.h"
#include "core/MovementRules.h"

// rules must refer to the same map as grid.
bool isGoalReachable(const Grid& grid, const MovementRules& rules);
