#include "generation/MazeGenerator.h"
#include "generation/MudSprinkler.h"

namespace {
constexpr int kDirections = 4;
constexpr int kDx[kDirections] = {1, -1, 0, 0};
constexpr int kDy[kDirections] = {0, 0, 1, -1};
constexpr int kCorridorStep = 2;
constexpr int kBorder = 1;

int lastOddCoordinate(int size) { return (size - 2) % 2 ? size - 2 : size - 3; }
}  // namespace

MazeGenerator::MazeGenerator(const GenerationSettings& settings, std::mt19937& rng)
    : settings_(settings), rng_(rng) {}

void MazeGenerator::generate(Grid& grid) {
    grid.fill(CellType::Wall);
    carvePassages(grid);
    placeEndpoints(grid);
    sprinkleMud(grid, settings_.mudDensity, rng_);
}

void MazeGenerator::carvePassages(Grid& grid) {
    std::vector<int> stack{grid.index(kBorder, kBorder)};
    grid.set(stack.back(), CellType::Empty);
    int options[kDirections];
    while (!stack.empty()) {
        int x = grid.xOf(stack.back()), y = grid.yOf(stack.back());
        int count = collectDirections(grid, x, y, options);
        if (count == 0) {
            stack.pop_back();
            continue;
        }
        int d = options[std::uniform_int_distribution<int>(0, count - 1)(rng_)];
        grid.set(grid.index(x + kDx[d], y + kDy[d]), CellType::Empty);
        int next = grid.index(x + kDx[d] * kCorridorStep, y + kDy[d] * kCorridorStep);
        grid.set(next, CellType::Empty);
        stack.push_back(next);
    }
}

int MazeGenerator::collectDirections(const Grid& grid, int x, int y, int* out) const {
    int count = 0;
    for (int d = 0; d < kDirections; d++) {
        int nx = x + kDx[d] * kCorridorStep, ny = y + kDy[d] * kCorridorStep;
        bool inside = nx >= kBorder && ny >= kBorder && nx < grid.width() - kBorder && ny < grid.height() - kBorder;
        if (inside && grid.at(grid.index(nx, ny)) == CellType::Wall) out[count++] = d;
    }
    return count;
}

void MazeGenerator::placeEndpoints(Grid& grid) {
    grid.setStart(grid.index(kBorder, kBorder));
    grid.setGoal(grid.index(lastOddCoordinate(grid.width()), lastOddCoordinate(grid.height())));
}
