#pragma once
#include "core/Grid.h"

struct GenerationSettings {
    float wallDensity = 0.30f;
    float mudDensity = 0.08f;
};

class IMapGenerator {
public:
    virtual ~IMapGenerator() = default;
    virtual void generate(Grid& grid) = 0;
};
