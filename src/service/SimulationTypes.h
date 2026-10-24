#pragma once
#include "algorithms/AlgorithmType.h"
#include "core/Heuristic.h"

struct SimulationSettings {
    AlgorithmType algorithm = AlgorithmType::AStar;
    HeuristicType heuristic = HeuristicType::Manhattan;
    float stepsPerSecond = 60.f;
    bool stepMode = false;
    bool agentEnabled = true;
    float agentSpeed = 8.f;
    bool soundEnabled = true;
};

enum class SimulationStatus { Idle, Running, Waiting, Found, NoPath };

struct SearchStats {
    int visitedCells = 0;
    int steps = 0;
    int pathLength = 0;
    float pathCost = 0.f;
    double elapsedMs = 0.0;
};
