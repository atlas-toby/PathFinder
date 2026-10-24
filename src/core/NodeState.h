#pragma once
#include <cstdint>

// A cell's state during a search. Dead: a dead end the random walk backtracked out of.
enum class NodeState : uint8_t { None, Open, Closed, Dead };
