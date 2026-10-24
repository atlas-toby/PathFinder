#include "algorithms/QuantumWalkPathfinder.h"
#include <cmath>

namespace {
constexpr float kTimePerStep = 1.f;
constexpr float kWaveDecay = 0.06f;
constexpr float kWaveFrequency = 1.3f;
}  // namespace

QuantumWalkPathfinder::QuantumWalkPathfinder(const Grid& grid, const MovementRules& rules)
    : grid_(grid), rules_(rules), state_(grid.cellCount(), grid.start()) {
    queue_.push({0.f, grid.start()});
}

void QuantumWalkPathfinder::step() {
    if (state_.isDone()) return;
    waveTime_ += kTimePerStep;
    while (!queue_.empty() && queue_.top().first <= waveTime_) {
        QueueEntry entry = queue_.top();
        queue_.pop();
        if (isStale(entry)) continue;
        state_.close(entry.second);
        if (entry.second == grid_.goal()) return state_.finish(true);
        expand(entry.second);
    }
    if (queue_.empty()) state_.finish(false);
}

std::optional<float> QuantumWalkPathfinder::waveIntensity(int cell) const {
    if (!state_.isClosed(cell)) return std::nullopt;
    float age = waveTime_ - state_.costTo(cell);
    return std::exp(-age * kWaveDecay) * (0.5f + 0.5f * std::cos(age * kWaveFrequency));
}

void QuantumWalkPathfinder::expand(int cell) {
    NeighborList neighbors;
    int count = rules_.neighbors(cell, neighbors);
    for (int k = 0; k < count; k++) {
        int next = neighbors[k].cell;
        float arrival = state_.costTo(cell) + neighbors[k].cost;
        if (state_.isClosed(next) || arrival >= state_.costTo(next)) continue;
        state_.open(next, cell, arrival);
        queue_.push({arrival, next});
    }
}

bool QuantumWalkPathfinder::isStale(const QueueEntry& entry) const {
    return state_.isClosed(entry.second) || entry.first > state_.costTo(entry.second);
}
