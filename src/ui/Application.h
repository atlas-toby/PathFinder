#pragma once
#include "config/AppConfig.h"
#include "ui/ControlPanel.h"
#include "ui/GridRenderer.h"
#include "ui/InputController.h"

struct GLFWwindow;

// Owns the GLFW window and Dear ImGui lifecycle; runs the main loop.
class Application {
public:
    Application(const AppConfig& config, const Grid& grid, SimulationService& simulation, ControlPanel& panel,
                GridRenderer& renderer, InputController& input);
    ~Application();
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run();

private:
    void createWindow();
    void initImGui();
    void loadFont();
    void renderFrame(float dt);

    const AppConfig& config_;
    const Grid& grid_;
    SimulationService& simulation_;
    ControlPanel& panel_;
    GridRenderer& renderer_;
    InputController& input_;
    GLFWwindow* window_ = nullptr;
};
