#pragma once
#include "algorithms/SearchState.h"
#include <optional>

// A pathfinding algorithm that can be advanced one step at a time.
class IPathfinder {
public:
    virtual ~IPathfinder() = default;
    virtual void step() = 0;
    virtual const SearchState& state() const = 0;
    // 0..1 brightness for wave-based algorithms; others return nothing.
    virtual std::optional<float> waveIntensity(int cell) const {
        (void)cell;
        return std::nullopt;
    }
};
