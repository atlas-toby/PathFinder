#pragma once
#include "service/SimulationService.h"
#include "ui/Brush.h"
#include "ui/GridLayout.h"

// Mouse map painting (left: brush, right: erase) and single step with Space.
class InputController {
public:
    InputController(Grid& grid, SimulationService& simulation);
    void handle(const GridLayout& layout, Brush brush);

private:
    void handleMouse(const GridLayout& layout, Brush brush);
    void handleKeyboard();
    bool paint(int cell, Brush brush);
    bool moveEndpoint(int cell, Brush brush);

    Grid& grid_;
    SimulationService& simulation_;
};
