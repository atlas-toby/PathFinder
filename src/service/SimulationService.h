#pragma once
#include "algorithms/PathfinderFactory.h"
#include "audio/ISoundPlayer.h"
#include "service/Agent.h"
#include "service/SimulationTypes.h"
#include <memory>
#include <vector>

// Search lifecycle: starting, stepping by speed, timing, sound and the agent.
class SimulationService {
public:
    SimulationService(const Grid& grid, const MovementRules& rules, const PathfinderFactory& factory,
                      ISoundPlayer& sound, SimulationSettings& settings);

    void start();
    void reset();
    void togglePause();
    void stepOnce();
    void solveInstantly();
    void update(float dt);
    void restartAgent() { agent_.restart(); }

    bool isPaused() const { return paused_; }
    SimulationStatus status() const;
    SearchStats stats() const;
    const IPathfinder* pathfinder() const { return pathfinder_.get(); }
    const std::vector<int>& path() const { return path_; }
    std::optional<CellPoint> agentPosition() const;

private:
    bool isActive() const;
    bool isSearching() const;
    void ensureStarted();
    void runTimedStep();
    void advanceByTime(float dt);
    void onSearchFinished();
    void updateSound();

    const Grid& grid_;
    const MovementRules& rules_;
    const PathfinderFactory& factory_;
    ISoundPlayer& sound_;
    SimulationSettings& settings_;

    std::unique_ptr<IPathfinder> pathfinder_;
    std::vector<int> path_;
    float pathCost_ = 0.f;
    double elapsedMs_ = 0.0;
    int steps_ = 0;
    float stepBudget_ = 0.f;
    bool paused_ = false;
    bool wasSearching_ = false;
    bool foundSoundPending_ = false;
    Agent agent_;
};
