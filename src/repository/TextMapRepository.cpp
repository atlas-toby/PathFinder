#include "repository/TextMapRepository.h"
#include "repository/MapFormatError.h"
#include <fstream>

namespace {
const std::string kMagic = "PF1";
const std::string kCellSymbols = ".#~";  // in CellType order: Empty, Wall, Mud

char symbolOf(CellType type) { return kCellSymbols[static_cast<int>(type)]; }
}  // namespace

void TextMapRepository::save(const Grid& grid, const std::string& path) {
    std::ofstream out(path);
    if (!out) throw MapFormatError(path, "could not open file for writing");
    out << kMagic << '\n' << grid.width() << ' ' << grid.height() << '\n'
        << grid.xOf(grid.start()) << ' ' << grid.yOf(grid.start()) << ' '
        << grid.xOf(grid.goal()) << ' ' << grid.yOf(grid.goal()) << '\n';
    for (int y = 0; y < grid.height(); y++) {
        for (int x = 0; x < grid.width(); x++) out << symbolOf(grid.at(grid.index(x, y)));
        out << '\n';
    }
    if (!out) throw MapFormatError(path, "error while writing");
}

Grid TextMapRepository::load(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw MapFormatError(path, "file not found or could not be opened");
    int start = 0, goal = 0;
    Grid grid = readHeader(in, path, start, goal);
    readRows(in, path, grid);
    grid.setStart(start);
    grid.setGoal(goal);
    return grid;
}

Grid TextMapRepository::readHeader(std::istream& in, const std::string& path, int& start, int& goal) const {
    std::string magic;
    int width, height, sx, sy, gx, gy;
    if (!(in >> magic >> width >> height >> sx >> sy >> gx >> gy) || magic != kMagic)
        throw MapFormatError(path, "could not read header (expected PF1 format)");
    if (width < Grid::kMinSize || height < Grid::kMinSize || width > Grid::kMaxSize || height > Grid::kMaxSize)
        throw MapFormatError(path, "invalid size " + std::to_string(width) + "x" + std::to_string(height));
    Grid grid(width, height);
    if (!grid.inBounds(sx, sy) || !grid.inBounds(gx, gy))
        throw MapFormatError(path, "start or goal is outside the map");
    start = grid.index(sx, sy);
    goal = grid.index(gx, gy);
    return grid;
}

void TextMapRepository::readRows(std::istream& in, const std::string& path, Grid& grid) const {
    for (int y = 0; y < grid.height(); y++) {
        std::string row;
        if (!(in >> row) || int(row.size()) != grid.width())
            throw MapFormatError(path, "row " + std::to_string(y + 1) + " has the wrong length");
        for (int x = 0; x < grid.width(); x++) {
            size_t symbol = kCellSymbols.find(row[x]);
            if (symbol == std::string::npos)
                throw MapFormatError(path, std::string("unknown cell character '") + row[x] + "'");
            grid.set(grid.index(x, y), static_cast<CellType>(symbol));
        }
    }
}
