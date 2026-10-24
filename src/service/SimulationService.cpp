#include "service/SimulationService.h"
#include <chrono>

namespace {
constexpr int kMaxStepsPerFrame = 50000;  // keeps the window responsive at very high speeds
}

SimulationService::SimulationService(const Grid& grid, const MovementRules& rules, const PathfinderFactory& factory,
                                     ISoundPlayer& sound, SimulationSettings& settings)
    : grid_(grid), rules_(rules), factory_(factory), sound_(sound), settings_(settings) {}

void SimulationService::start() {
    reset();
    pathfinder_ = factory_.create(settings_.algorithm, settings_.heuristic);
    updateSound();
}

void SimulationService::reset() {
    pathfinder_.reset();
    path_.clear();
    pathCost_ = 0.f;
    elapsedMs_ = 0.0;
    steps_ = 0;
    stepBudget_ = 0.f;
    paused_ = false;
    foundSoundPending_ = false;
    agent_.restart();
    updateSound();
}

void SimulationService::togglePause() {
    paused_ = !paused_;
    updateSound();
}

void SimulationService::stepOnce() {
    ensureStarted();
    settings_.stepMode = true;
    runTimedStep();
    updateSound();
}

void SimulationService::solveInstantly() {
    ensureStarted();
    while (!pathfinder_->state().isDone()) runTimedStep();
    updateSound();
}

void SimulationService::update(float dt) {
    if (isSearching()) advanceByTime(dt);
    if (status() == SimulationStatus::Found && settings_.agentEnabled)
        agent_.advance(dt, settings_.agentSpeed, path_, rules_);
    updateSound();
}

SimulationStatus SimulationService::status() const {
    if (!pathfinder_) return SimulationStatus::Idle;
    const SearchState& state = pathfinder_->state();
    if (state.isDone()) return state.isFound() ? SimulationStatus::Found : SimulationStatus::NoPath;
    return isSearching() ? SimulationStatus::Running : SimulationStatus::Waiting;
}

SearchStats SimulationService::stats() const {
    SearchStats result;
    result.visitedCells = pathfinder_ ? pathfinder_->state().visitedCount() : 0;
    result.steps = steps_;
    result.pathLength = path_.empty() ? 0 : int(path_.size()) - 1;
    result.pathCost = pathCost_;
    result.elapsedMs = elapsedMs_;
    return result;
}

std::optional<CellPoint> SimulationService::agentPosition() const {
    if (!settings_.agentEnabled) return std::nullopt;
    return agent_.position(path_, grid_);
}

bool SimulationService::isActive() const { return pathfinder_ && !pathfinder_->state().isDone(); }

bool SimulationService::isSearching() const { return isActive() && !paused_ && !settings_.stepMode; }

void SimulationService::ensureStarted() {
    if (!isActive()) start();
}

void SimulationService::runTimedStep() {
    auto begin = std::chrono::steady_clock::now();
    pathfinder_->step();
    elapsedMs_ += std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - begin).count();
    steps_++;
    if (pathfinder_->state().isDone()) onSearchFinished();
}

void SimulationService::advanceByTime(float dt) {
    stepBudget_ += dt * settings_.stepsPerSecond;
    for (int k = 0; stepBudget_ >= 1.f && isActive() && k < kMaxStepsPerFrame; k++) {
        runTimedStep();
        stepBudget_ -= 1.f;
    }
    if (!isActive()) stepBudget_ = 0.f;
}

void SimulationService::onSearchFinished() {
    path_ = pathfinder_->state().pathTo(grid_.goal());
    pathCost_ = rules_.pathCost(path_);
    foundSoundPending_ = pathfinder_->state().isFound();
}

// The search sound plays only while the search is actually running; the found sound interrupts it once.
void SimulationService::updateSound() {
    bool searching = isSearching() && settings_.soundEnabled;
    if (searching != wasSearching_) searching ? sound_.playLoop(SoundId::Searching) : sound_.stop();
    wasSearching_ = searching;
    if (!foundSoundPending_) return;
    foundSoundPending_ = false;
    if (settings_.soundEnabled) sound_.playOnce(SoundId::Found);
}
