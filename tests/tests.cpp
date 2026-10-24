// Framework-free unit tests: failed checks are reported and reflected in the exit code.
#include "algorithms/PathfinderFactory.h"
#include "config/ConfigError.h"
#include "config/IniConfigLoader.h"
#include "core/Reachability.h"
#include "generation/MazeGenerator.h"
#include "generation/RandomMapGenerator.h"
#include "repository/MapFormatError.h"
#include "repository/TextMapRepository.h"
#include "service/SimulationService.h"
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace {
int failures = 0;

void check(bool condition, const char* what) {
    if (condition) return;
    std::printf("FAILED: %s\n", what);
    failures++;
}

template <typename Error, typename Action>
bool throws(Action action) {
    try {
        action();
    } catch (const Error&) {
        return true;
    }
    return false;
}

class FakeSoundPlayer : public ISoundPlayer {
public:
    void playLoop(SoundId) override { loops++; }
    void playOnce(SoundId sound) override { foundPlayed = sound == SoundId::Found; }
    void stop() override { stops++; }
    int loops = 0, stops = 0;
    bool foundPlayed = false;
};

struct World {
    std::mt19937 rng{42};
    Grid grid{41, 31};
    MovementSettings movement;
    GenerationSettings generation;
    MovementRules rules{grid, movement};
    PathfinderFactory factory{grid, rules, rng};
};

std::vector<int> solve(World& world, AlgorithmType algorithm, float& cost) {
    auto pathfinder = world.factory.create(algorithm, HeuristicType::Euclidean);
    while (!pathfinder->state().isDone()) pathfinder->step();
    std::vector<int> path = pathfinder->state().pathTo(world.grid.goal());
    cost = world.rules.pathCost(path);
    return path;
}

void testAllAlgorithmsFindPaths() {
    World world;
    RandomMapGenerator random(world.rules, world.generation, world.rng);
    MazeGenerator maze(world.generation, world.rng);
    for (int trial = 0; trial < 40; trial++) {
        world.movement.allowDiagonal = trial % 2;
        trial % 3 ? random.generate(world.grid) : maze.generate(world.grid);
        float dijkstraCost = 0.f;
        for (int a = 0; a < static_cast<int>(AlgorithmType::Count); a++) {
            float cost = 0.f;
            auto path = solve(world, static_cast<AlgorithmType>(a), cost);
            check(!path.empty() && path.front() == world.grid.start() && path.back() == world.grid.goal(),
                  "every algorithm should find a path from start to goal");
            if (static_cast<AlgorithmType>(a) == AlgorithmType::Dijkstra) dijkstraCost = cost;
            if (static_cast<AlgorithmType>(a) == AlgorithmType::QuantumWalk)
                check(std::fabs(cost - dijkstraCost) < 1e-3f, "Quantum should give the same optimal cost as Dijkstra");
        }
    }
}

void testUnreachableGoal() {
    World world;
    world.grid.set(world.grid.goal(), CellType::Wall);
    check(!isGoalReachable(world.grid, world.rules), "a walled-in goal should be unreachable");
    float cost = 0.f;
    check(solve(world, AlgorithmType::AStar, cost).empty(), "no path should be found to an unreachable goal");
}

void testRepositoryRoundTrip() {
    World world;
    MazeGenerator(world.generation, world.rng).generate(world.grid);
    std::string path = (std::filesystem::temp_directory_path() / "pathfinder_test_map.txt").string();
    TextMapRepository repository;
    repository.save(world.grid, path);
    Grid loaded = repository.load(path);
    bool same = loaded.width() == world.grid.width() && loaded.start() == world.grid.start() &&
                loaded.goal() == world.grid.goal();
    for (int cell = 0; same && cell < loaded.cellCount(); cell++) same = loaded.at(cell) == world.grid.at(cell);
    check(same, "a saved and reloaded map should be identical");
    std::ofstream(path) << "PF1\n5 5\n0 0 4 4\n.....\n..X..\n";
    check(throws<MapFormatError>([&] { repository.load(path); }), "a malformed file should throw MapFormatError");
    check(throws<MapFormatError>([&] { repository.load(path + ".missing"); }), "a missing file should throw MapFormatError");
    std::filesystem::remove(path);
}

void testSimulationPlaysSounds() {
    World world;
    FakeSoundPlayer sound;
    SimulationSettings settings;
    SimulationService simulation(world.grid, world.rules, world.factory, sound, settings);
    simulation.start();
    check(sound.loops == 1, "the search sound should loop when the search starts");
    simulation.update(1000.f);
    check(simulation.status() == SimulationStatus::Found, "a path should be found on an empty map");
    check(sound.stops >= 1 && sound.foundPlayed, "on success the search sound should stop and the found sound play");
    check(simulation.stats().pathLength == world.grid.width() - 3, "the path on an empty map should be a straight line");
}

void testConfigLoader() {
    IniConfigLoader loader;
    std::istringstream valid("# comment\ngrid_width = 55\nfont_path = a/b.ttf\n");
    AppConfig config = loader.parse(valid, "test");
    check(config.gridWidth == 55 && config.fontPath == "a/b.ttf", "valid settings should be read");
    check(config.gridHeight == AppConfig{}.gridHeight, "a missing key should keep its default");
    std::istringstream badNumber("grid_width = abc\n"), unknownKey("color = blue\n");
    check(throws<ConfigError>([&] { loader.parse(badNumber, "test"); }), "a non-numeric value should throw ConfigError");
    check(throws<ConfigError>([&] { loader.parse(unknownKey, "test"); }), "an unknown key should throw ConfigError");
}
}  // namespace

int main() {
    testAllAlgorithmsFindPaths();
    testUnreachableGoal();
    testRepositoryRoundTrip();
    testSimulationPlaysSounds();
    testConfigLoader();
    if (failures == 0) {
        std::printf("All tests passed\n");
        return 0;
    }
    std::printf("%d test checks failed\n", failures);
    return 1;
}
