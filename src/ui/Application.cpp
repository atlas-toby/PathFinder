#include "ui/Application.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

namespace {
constexpr const char* kWindowTitle = "2D Pathfinding";
constexpr ImWchar kLatinGlyphRanges[] = {0x0020, 0x00FF, 0x0100, 0x017F, 0};  // Basic Latin + Latin-1 + Latin Extended-A
}  // namespace

Application::Application(const AppConfig& config, const Grid& grid, SimulationService& simulation,
                         ControlPanel& panel, GridRenderer& renderer, InputController& input)
    : config_(config), grid_(grid), simulation_(simulation), panel_(panel), renderer_(renderer), input_(input) {
    createWindow();
    initImGui();
}

Application::~Application() {
    simulation_.reset();  // stops the looping sound
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window_);
    glfwTerminate();
}

void Application::run() {
    double last = glfwGetTime();
    while (!glfwWindowShouldClose(window_)) {
        glfwPollEvents();
        double now = glfwGetTime();
        renderFrame(float(now - last));
        last = now;
        glfwSwapBuffers(window_);
    }
}

void Application::createWindow() {
    if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW");
    window_ = glfwCreateWindow(config_.windowWidth, config_.windowHeight, kWindowTitle, nullptr, nullptr);
    if (!window_) {
        glfwTerminate();
        throw std::runtime_error("Failed to create the OpenGL window");
    }
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);
}

void Application::initImGui() {
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    loadFont();
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL2_Init();
}

void Application::loadFont() {
    if (!std::filesystem::exists(config_.fontPath)) return;  // otherwise ImGui's default font is used
    ImGui::GetIO().Fonts->AddFontFromFileTTF(config_.fontPath.c_str(), config_.fontSize, nullptr,
                                             kLatinGlyphRanges);
}

void Application::renderFrame(float dt) {
    int viewWidth, viewHeight, framebufferWidth, framebufferHeight;
    glfwGetWindowSize(window_, &viewWidth, &viewHeight);
    glfwGetFramebufferSize(window_, &framebufferWidth, &framebufferHeight);
    float gridAreaWidth = std::max(1.f, viewWidth - config_.panelWidth);
    GridLayout layout = GridLayout::fit(grid_, gridAreaWidth, float(viewHeight));

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    input_.handle(layout, panel_.brush());
    simulation_.update(dt);
    panel_.draw(viewWidth - config_.panelWidth, config_.panelWidth, float(viewHeight));

    renderer_.draw(layout, framebufferWidth, framebufferHeight, float(viewWidth), float(viewHeight));
    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}
