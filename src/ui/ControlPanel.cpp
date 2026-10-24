#include "ui/ControlPanel.h"
#include "core/Reachability.h"
#include "imgui.h"
#include <cstring>
#include <filesystem>
#include <stdexcept>

namespace {
constexpr ImGuiWindowFlags kPanelFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
constexpr ImGuiTreeNodeFlags kOpen = ImGuiTreeNodeFlags_DefaultOpen;
constexpr float kMaxWallDensity = 0.6f, kMaxMudDensity = 0.5f;
constexpr float kMinMudCost = 1.f, kMaxMudCost = 20.f;
constexpr float kMinSpeed = 1.f, kMaxSpeed = 20000.f;
constexpr float kMinAgentSpeed = 1.f, kMaxAgentSpeed = 60.f;
constexpr int kMaxEditableGridSize = 200;
const ImVec4 kWarningColor(1.f, 0.35f, 0.3f, 1.f);

const char* statusText(SimulationStatus status) {
    switch (status) {
        case SimulationStatus::Running: return "Running";
        case SimulationStatus::Waiting: return "Waiting";
        case SimulationStatus::Found: return "Path found";
        case SimulationStatus::NoPath: return "No path";
        default: return "Ready";
    }
}

template <typename Enum>
bool enumCombo(const char* label, Enum& value, const char* const* names) {
    int index = static_cast<int>(value);
    if (!ImGui::Combo(label, &index, names, static_cast<int>(Enum::Count))) return false;
    value = static_cast<Enum>(index);
    return true;
}
}  // namespace

ControlPanel::ControlPanel(ControlPanelDependencies deps, std::string mapDirectory, const std::string& mapFile)
    : deps_(deps), mapDirectory_(std::move(mapDirectory)), pendingWidth_(deps.grid.width()),
      pendingHeight_(deps.grid.height()) {
    std::strncpy(mapFile_, mapFile.c_str(), sizeof mapFile_ - 1);
}

void ControlPanel::draw(float left, float width, float height) {
    ImGui::SetNextWindowPos(ImVec2(left, 0));
    ImGui::SetNextWindowSize(ImVec2(width, height));
    ImGui::Begin("Controls", nullptr, kPanelFlags);
    drawMapSection();
    drawAlgorithmSection();
    drawSimulationSection();
    drawStatsSection();
    drawColorSection();
    drawFileSection();
    ImGui::End();
}

void ControlPanel::drawMapSection() {
    if (!ImGui::CollapsingHeader("Map", kOpen)) return;
    drawBrushSelector();
    ImGui::SliderFloat("Wall density", &deps_.generation.wallDensity, 0.f, kMaxWallDensity, "%.2f");
    ImGui::SliderFloat("Mud density", &deps_.generation.mudDensity, 0.f, kMaxMudDensity, "%.2f");
    if (ImGui::SliderFloat("Mud cost", &deps_.movement.mudCost, kMinMudCost, kMaxMudCost, "%.1f"))
        deps_.simulation.reset();
    drawGeneratorButtons();
    drawResizeControls();
    if (!isGoalReachable(deps_.grid, deps_.rules))
        ImGui::TextColored(kWarningColor, "WARNING: The goal is unreachable!\n(You can still run the simulation.)");
}

void ControlPanel::drawBrushSelector() {
    ImGui::TextDisabled("Left click: brush, right click: erase");
    for (int k = 0; k < static_cast<int>(Brush::Count); k++) {
        if (k) ImGui::SameLine();
        if (ImGui::RadioButton(kBrushNames[k], brush_ == static_cast<Brush>(k))) brush_ = static_cast<Brush>(k);
    }
}

void ControlPanel::drawGeneratorButtons() {
    IMapGenerator* chosen = nullptr;
    if (ImGui::Button("Random map")) chosen = &deps_.randomGenerator;
    ImGui::SameLine();
    if (ImGui::Button("Generate maze")) chosen = &deps_.mazeGenerator;
    ImGui::SameLine();
    if (ImGui::Button("Clear")) {
        deps_.grid.fill(CellType::Empty);
        deps_.grid.placeDefaultEndpoints();
        deps_.simulation.reset();
    }
    if (!chosen) return;
    chosen->generate(deps_.grid);
    deps_.simulation.reset();
}

void ControlPanel::drawResizeControls() {
    ImGui::SliderInt("Width", &pendingWidth_, Grid::kMinSize, kMaxEditableGridSize);
    ImGui::SliderInt("Height", &pendingHeight_, Grid::kMinSize, kMaxEditableGridSize);
    if (!ImGui::Button("Apply size")) return;
    deps_.grid.resize(pendingWidth_, pendingHeight_);
    deps_.simulation.reset();
}

void ControlPanel::drawAlgorithmSection() {
    if (!ImGui::CollapsingHeader("Algorithm", kOpen)) return;
    SimulationSettings& settings = deps_.simulationSettings;
    bool changed = enumCombo("Algorithm##select", settings.algorithm, kAlgorithmNames);
    ImGui::BeginDisabled(!usesHeuristic(settings.algorithm));
    changed |= enumCombo("Heuristic", settings.heuristic, kHeuristicNames);
    ImGui::EndDisabled();
    changed |= ImGui::Checkbox("8-directional movement", &deps_.movement.allowDiagonal);
    ImGui::BeginDisabled(!deps_.movement.allowDiagonal);
    changed |= ImGui::Checkbox("Prevent corner cutting", &deps_.movement.preventCornerCutting);
    ImGui::EndDisabled();
    if (changed) deps_.simulation.reset();
}

void ControlPanel::drawSimulationButtons() {
    SimulationService& simulation = deps_.simulation;
    if (ImGui::Button("Start")) {
        deps_.simulationSettings.stepMode = false;
        simulation.start();
    }
    ImGui::SameLine();
    if (ImGui::Button(simulation.isPaused() ? "Resume" : "Pause")) simulation.togglePause();
    ImGui::SameLine();
    if (ImGui::Button("Solve instantly")) simulation.solveInstantly();
    ImGui::SameLine();
    if (ImGui::Button("Reset")) simulation.reset();
}

void ControlPanel::drawSimulationSection() {
    if (!ImGui::CollapsingHeader("Simulation", kOpen)) return;
    SimulationSettings& settings = deps_.simulationSettings;
    drawSimulationButtons();
    ImGui::Checkbox("Step mode (Space = 1 step)", &settings.stepMode);
    ImGui::SliderFloat("Speed (steps/s)", &settings.stepsPerSecond, kMinSpeed, kMaxSpeed, "%.0f",
                       ImGuiSliderFlags_Logarithmic);
    ImGui::Checkbox("Sound effects", &settings.soundEnabled);
    ImGui::Checkbox("Moving agent", &settings.agentEnabled);
    ImGui::SameLine();
    if (ImGui::Button("Replay agent")) deps_.simulation.restartAgent();
    ImGui::SliderFloat("Agent speed (cells/s)", &settings.agentSpeed, kMinAgentSpeed, kMaxAgentSpeed, "%.1f");
}

void ControlPanel::drawStatsSection() {
    if (!ImGui::CollapsingHeader("Statistics", kOpen)) return;
    SearchStats stats = deps_.simulation.stats();
    ImGui::Text("Status: %s", statusText(deps_.simulation.status()));
    ImGui::Text("Visited cells: %d", stats.visitedCells);
    ImGui::Text("Path length: %d cells", stats.pathLength);
    ImGui::Text("Total cost: %.2f", stats.pathCost);
    ImGui::Text("Time: %.3f ms  (%d steps)", stats.elapsedMs, stats.steps);
}

void ControlPanel::drawColorSection() {
    if (!ImGui::CollapsingHeader("Colors")) return;
    ColorTheme& c = deps_.colors;
    ImGui::PushID("colors");  // keeps labels from clashing with the brush buttons
    ImGui::ColorEdit3("Background", c.background.data());
    ImGui::ColorEdit3("Empty", c.empty.data());
    ImGui::ColorEdit3("Wall", c.wall.data());
    ImGui::ColorEdit3("Mud", c.mud.data());
    ImGui::ColorEdit3("Open set", c.open.data());
    ImGui::ColorEdit3("Visited", c.visited.data());
    ImGui::ColorEdit3("Dead end (Random)", c.deadEnd.data());
    ImGui::ColorEdit3("Path", c.path.data());
    ImGui::ColorEdit3("Start", c.start.data());
    ImGui::ColorEdit3("Goal", c.goal.data());
    ImGui::ColorEdit3("Agent", c.agent.data());
    ImGui::PopID();
}

void ControlPanel::drawFileSection() {
    if (!ImGui::CollapsingHeader("Save / Load", kOpen)) return;
    ImGui::InputText("File", mapFile_, sizeof mapFile_);
    if (ImGui::Button("Save")) saveMap();
    ImGui::SameLine();
    if (ImGui::Button("Load")) loadMap();
    if (!message_.empty()) ImGui::TextWrapped("%s", message_.c_str());
}

void ControlPanel::saveMap() {
    try {
        deps_.repository.save(deps_.grid, resolveMapPath());
        message_ = "Saved.";
    } catch (const std::exception& error) {
        message_ = std::string("Save failed: ") + error.what();
    }
}

void ControlPanel::loadMap() {
    try {
        deps_.grid = deps_.repository.load(resolveMapPath());
        pendingWidth_ = deps_.grid.width();
        pendingHeight_ = deps_.grid.height();
        deps_.simulation.reset();
        message_ = "Loaded.";
    } catch (const std::exception& error) {
        message_ = std::string("Load failed: ") + error.what();
    }
}

std::string ControlPanel::resolveMapPath() const {
    std::filesystem::path path(mapFile_);
    return path.is_relative() ? (std::filesystem::path(mapDirectory_) / path).string() : path.string();
}
