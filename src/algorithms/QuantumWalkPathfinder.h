#pragma once
#include "algorithms/IPathfinder.h"
#include "algorithms/MinQueue.h"
#include "core/Grid.h"
#include "core/MovementRules.h"

// Instead of following a single path, spreads over the whole grid like a wave: each step advances time
// by one unit and every cell whose arrival time has come opens at once. Mud slows the wave down.
class QuantumWalkPathfinder : public IPathfinder {
public:
    QuantumWalkPathfinder(const Grid& grid, const MovementRules& rules);

    void step() override;
    const SearchState& state() const override { return state_; }
    std::optional<float> waveIntensity(int cell) const override;

private:
    void expand(int cell);
    bool isStale(const QueueEntry& entry) const;

    const Grid& grid_;
    const MovementRules& rules_;
    SearchState state_;
    MinQueue queue_;
    float waveTime_ = 0.f;
};
