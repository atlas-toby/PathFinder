#pragma once
#include "core/Grid.h"

enum class HeuristicType { Manhattan, Euclidean, Chebyshev, Count };

inline constexpr const char* kHeuristicNames[] = {"Manhattan", "Euclidean", "Chebyshev"};

float estimateDistance(HeuristicType type, const Grid& grid, int from, int to);
