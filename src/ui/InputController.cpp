#include "ui/InputController.h"
#include "imgui.h"

namespace {
constexpr int kLeftButton = 0;
constexpr int kRightButton = 1;

CellType cellTypeOf(Brush brush) {
    switch (brush) {
        case Brush::Wall: return CellType::Wall;
        case Brush::Mud: return CellType::Mud;
        default: return CellType::Empty;
    }
}
}  // namespace

InputController::InputController(Grid& grid, SimulationService& simulation) : grid_(grid), simulation_(simulation) {}

void InputController::handle(const GridLayout& layout, Brush brush) {
    handleMouse(layout, brush);
    handleKeyboard();
}

void InputController::handleMouse(const GridLayout& layout, Brush brush) {
    const ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;
    bool erasing = ImGui::IsMouseDown(kRightButton);
    if (!erasing && !ImGui::IsMouseDown(kLeftButton)) return;
    auto cell = layout.cellAt(grid_, io.MousePos.x, io.MousePos.y);
    if (!cell) return;
    if (paint(*cell, erasing ? Brush::Erase : brush)) simulation_.reset();
}

void InputController::handleKeyboard() {
    if (ImGui::GetIO().WantCaptureKeyboard) return;
    if (ImGui::IsKeyPressed(ImGuiKey_Space)) simulation_.stepOnce();
}

bool InputController::paint(int cell, Brush brush) {
    if (brush == Brush::Start || brush == Brush::Goal) return moveEndpoint(cell, brush);
    CellType type = cellTypeOf(brush);
    if (grid_.isEndpoint(cell) || grid_.at(cell) == type) return false;
    grid_.set(cell, type);
    return true;
}

bool InputController::moveEndpoint(int cell, Brush brush) {
    if (grid_.isEndpoint(cell)) return false;
    brush == Brush::Start ? grid_.setStart(cell) : grid_.setGoal(cell);
    return true;
}
