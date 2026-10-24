#pragma once
#include "core/Grid.h"
#include <string>

class IMapRepository {
public:
    virtual ~IMapRepository() = default;
    virtual void save(const Grid& grid, const std::string& path) = 0;
    virtual Grid load(const std::string& path) = 0;
};
