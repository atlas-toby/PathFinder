#include "algorithms/SearchState.h"
#include <algorithm>
#include <limits>

namespace {
constexpr float kUnknownCost = std::numeric_limits<float>::infinity();
constexpr int kNoParent = -1;
}  // namespace

SearchState::SearchState(int cellCount, int start)
    : states_(cellCount, NodeState::None), costs_(cellCount, kUnknownCost), parents_(cellCount, kNoParent) {
    open(start, kNoParent, 0.f);
}

bool SearchState::hasCost(int cell) const { return costs_[cell] != kUnknownCost; }

void SearchState::open(int cell, int parent, float cost) {
    states_[cell] = NodeState::Open;
    parents_[cell] = parent;
    costs_[cell] = cost;
}

void SearchState::close(int cell) {
    if (states_[cell] == NodeState::Closed) return;
    states_[cell] = NodeState::Closed;
    visited_++;
}

void SearchState::finish(bool found) {
    done_ = true;
    found_ = found;
}

std::vector<int> SearchState::pathTo(int goal) const {
    std::vector<int> path;
    if (!found_) return path;
    for (int cell = goal; cell != kNoParent; cell = parents_[cell]) path.push_back(cell);
    std::reverse(path.begin(), path.end());
    return path;
}
