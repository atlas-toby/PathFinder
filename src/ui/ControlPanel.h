#pragma once
#include "generation/IMapGenerator.h"
#include "repository/IMapRepository.h"
#include "service/SimulationService.h"
#include "ui/Brush.h"
#include "ui/ColorTheme.h"
#include <string>

struct ControlPanelDependencies {
    Grid& grid;
    const MovementRules& rules;
    MovementSettings& movement;
    GenerationSettings& generation;
    SimulationSettings& simulationSettings;
    SimulationService& simulation;
    IMapGenerator& randomGenerator;
    IMapGenerator& mazeGenerator;
    IMapRepository& repository;
    ColorTheme& colors;
};

// The Dear ImGui settings panel on the right.
class ControlPanel {
public:
    ControlPanel(ControlPanelDependencies deps, std::string mapDirectory, const std::string& mapFile);
    void draw(float left, float width, float height);
    Brush brush() const { return brush_; }

private:
    void drawMapSection();
    void drawBrushSelector();
    void drawGeneratorButtons();
    void drawResizeControls();
    void drawAlgorithmSection();
    void drawSimulationButtons();
    void drawSimulationSection();
    void drawStatsSection();
    void drawColorSection();
    void drawFileSection();
    void saveMap();
    void loadMap();
    std::string resolveMapPath() const;

    ControlPanelDependencies deps_;
    std::string mapDirectory_;
    Brush brush_ = Brush::Wall;
    int pendingWidth_, pendingHeight_;
    char mapFile_[256] = {};
    std::string message_;
};
