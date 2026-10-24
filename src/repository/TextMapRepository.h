#pragma once
#include "repository/IMapRepository.h"
#include <istream>

// Format: "PF1", "W H", "sx sy gx gy", then H rows ('.' empty, '#' wall, '~' mud).
class TextMapRepository : public IMapRepository {
public:
    void save(const Grid& grid, const std::string& path) override;
    Grid load(const std::string& path) override;

private:
    Grid readHeader(std::istream& in, const std::string& path, int& start, int& goal) const;
    void readRows(std::istream& in, const std::string& path, Grid& grid) const;
};
