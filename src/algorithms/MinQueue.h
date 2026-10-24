#pragma once
#include <functional>
#include <queue>
#include <utility>
#include <vector>

// (priority, cell) pairs; the smallest priority comes out first.
using QueueEntry = std::pair<float, int>;
using MinQueue = std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<>>;
