#include "ui/GridRenderer.h"
#include <GLFW/glfw3.h>
#include <cmath>

namespace {
constexpr float kMinCellSizeForGap = 6.f;
constexpr float kCellGap = 1.f;
constexpr float kVisitedBlend = 0.7f;
constexpr float kOpenBlend = 0.8f;
constexpr float kDeadBlend = 0.7f;
constexpr float kWaveBaseBlend = 0.45f;
constexpr float kAgentRadius = 0.4f;
constexpr int kAgentSegments = 24;
constexpr float kTwoPi = 6.2831853f;
}  // namespace

GridRenderer::GridRenderer(const Grid& grid, const SimulationService& simulation, const ColorTheme& colors)
    : grid_(grid), simulation_(simulation), colors_(colors) {}

void GridRenderer::draw(const GridLayout& layout, int framebufferWidth, int framebufferHeight, float viewWidth,
                        float viewHeight) {
    clearAndProject(framebufferWidth, framebufferHeight, viewWidth, viewHeight);
    markPathCells();
    drawCells(layout);
    drawAgent(layout);
}

void GridRenderer::clearAndProject(int framebufferWidth, int framebufferHeight, float viewWidth,
                                   float viewHeight) const {
    glViewport(0, 0, framebufferWidth, framebufferHeight);
    glClearColor(colors_.background[0], colors_.background[1], colors_.background[2], 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, viewWidth, viewHeight, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void GridRenderer::markPathCells() {
    onPath_.assign(grid_.cellCount(), false);
    for (int cell : simulation_.path()) onPath_[cell] = true;
}

void GridRenderer::drawCells(const GridLayout& layout) {
    float gap = layout.cellSize >= kMinCellSizeForGap ? kCellGap : 0.f;
    glBegin(GL_QUADS);
    for (int cell = 0; cell < grid_.cellCount(); cell++) {
        float left = layout.toPixelX(float(grid_.xOf(cell))), top = layout.toPixelY(float(grid_.yOf(cell)));
        float right = left + layout.cellSize, bottom = top + layout.cellSize;
        glColor3fv(cellColor(cell).data());
        glVertex2f(left + gap, top + gap);
        glVertex2f(right, top + gap);
        glVertex2f(right, bottom);
        glVertex2f(left + gap, bottom);
    }
    glEnd();
}

void GridRenderer::drawAgent(const GridLayout& layout) const {
    auto position = simulation_.agentPosition();
    if (!position) return;
    float centerX = layout.toPixelX(position->x), centerY = layout.toPixelY(position->y);
    float radius = layout.cellSize * kAgentRadius;
    glColor3fv(colors_.agent.data());
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(centerX, centerY);
    for (int k = 0; k <= kAgentSegments; k++) {
        float angle = k * kTwoPi / kAgentSegments;
        glVertex2f(centerX + std::cos(angle) * radius, centerY + std::sin(angle) * radius);
    }
    glEnd();
}

Color GridRenderer::baseColor(int cell) const {
    switch (grid_.at(cell)) {
        case CellType::Wall: return colors_.wall;
        case CellType::Mud: return colors_.mud;
        default: return colors_.empty;
    }
}

Color GridRenderer::searchColor(int cell, const Color& base) const {
    const IPathfinder* pathfinder = simulation_.pathfinder();
    if (auto wave = pathfinder->waveIntensity(cell))
        return mixColors(mixColors(base, colors_.visited, kWaveBaseBlend), colors_.open, *wave);
    switch (pathfinder->state().stateOf(cell)) {
        case NodeState::Closed: return mixColors(base, colors_.visited, kVisitedBlend);
        case NodeState::Open: return mixColors(base, colors_.open, kOpenBlend);
        case NodeState::Dead: return mixColors(base, colors_.deadEnd, kDeadBlend);
        default: return base;
    }
}

Color GridRenderer::cellColor(int cell) const {
    if (cell == grid_.start()) return colors_.start;
    if (cell == grid_.goal()) return colors_.goal;
    if (onPath_[cell]) return colors_.path;
    Color base = baseColor(cell);
    if (!simulation_.pathfinder() || grid_.at(cell) == CellType::Wall) return base;
    return searchColor(cell, base);
}
