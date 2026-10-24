#pragma once
#include "core/NodeState.h"
#include <vector>

// Shared bookkeeping for all algorithms: cell states, costs, parents.
class SearchState {
public:
    SearchState(int cellCount, int start);

    NodeState stateOf(int cell) const { return states_[cell]; }
    bool isUnseen(int cell) const { return states_[cell] == NodeState::None; }
    bool isClosed(int cell) const { return states_[cell] == NodeState::Closed; }
    float costTo(int cell) const { return costs_[cell]; }
    bool hasCost(int cell) const;

    void open(int cell, int parent, float cost);
    void close(int cell);
    void markDead(int cell) { states_[cell] = NodeState::Dead; }
    void finish(bool found);

    bool isDone() const { return done_; }
    bool isFound() const { return found_; }
    int visitedCount() const { return visited_; }
    std::vector<int> pathTo(int goal) const;

private:
    std::vector<NodeState> states_;
    std::vector<float> costs_;
    std::vector<int> parents_;
    int visited_ = 0;
    bool done_ = false, found_ = false;
};
