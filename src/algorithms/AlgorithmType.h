#pragma once

enum class AlgorithmType { AStar, Dijkstra, Bfs, Greedy, RandomWalk, QuantumWalk, Count };

inline constexpr const char* kAlgorithmNames[] = {"A*", "Dijkstra", "BFS", "Greedy Best-First", "Random",
                                                  "Quantum Walk"};

inline bool usesHeuristic(AlgorithmType type) { return type == AlgorithmType::AStar || type == AlgorithmType::Greedy; }
