#include "algorithms/PathfinderFactory.h"
#include "audio/WinSoundPlayer.h"
#include "config/ConfigError.h"
#include "config/ExecutableDirectory.h"
#include "config/IniConfigLoader.h"
#include "generation/MazeGenerator.h"
#include "generation/RandomMapGenerator.h"
#include "repository/TextMapRepository.h"
#include "service/SimulationService.h"
#include "ui/Application.h"
#include <windows.h>

namespace {
constexpr const char* kConfigFile = "config/app.ini";
constexpr const char* kAppName = "Pathfinder";

void showError(const char* message, UINT icon) { MessageBoxA(nullptr, message, kAppName, icon); }

AppConfig loadConfigOrDefaults(const std::string& baseDirectory) {
    try {
        return IniConfigLoader().load(baseDirectory + kConfigFile);
    } catch (const ConfigError& error) {
        showError((std::string(error.what()) + "\nDefault settings will be used.").c_str(), MB_ICONWARNING);
        return AppConfig{};
    }
}

void run() {
    const std::string baseDirectory = executableDirectory();
    const AppConfig config = loadConfigOrDefaults(baseDirectory);

    std::mt19937 rng{std::random_device{}()};
    Grid grid(config.gridWidth, config.gridHeight);
    MovementSettings movement{false, true, config.mudCost};
    GenerationSettings generation{config.wallDensity, config.mudDensity};
    SimulationSettings simulationSettings;
    simulationSettings.stepsPerSecond = config.stepsPerSecond;
    simulationSettings.agentSpeed = config.agentSpeed;
    ColorTheme colors;

    MovementRules rules(grid, movement);
    PathfinderFactory factory(grid, rules, rng);
    WinSoundPlayer sound({baseDirectory + config.searchSound, baseDirectory + config.foundSound});
    SimulationService simulation(grid, rules, factory, sound, simulationSettings);
    RandomMapGenerator randomGenerator(rules, generation, rng);
    MazeGenerator mazeGenerator(generation, rng);
    TextMapRepository repository;

    ControlPanel panel({grid, rules, movement, generation, simulationSettings, simulation, randomGenerator,
                        mazeGenerator, repository, colors},
                       baseDirectory, config.mapFile);
    GridRenderer renderer(grid, simulation, colors);
    InputController input(grid, simulation);
    Application(config, grid, simulation, panel, renderer, input).run();
}
}  // namespace

int main() {
    try {
        run();
        return 0;
    } catch (const std::exception& error) {
        showError(error.what(), MB_ICONERROR);
        return 1;
    }
}
