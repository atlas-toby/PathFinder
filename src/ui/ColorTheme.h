#pragma once
#include <array>

using Color = std::array<float, 3>;

struct ColorTheme {
    Color background{0.08f, 0.08f, 0.10f};
    Color empty{0.92f, 0.92f, 0.94f};
    Color wall{0.15f, 0.17f, 0.22f};
    Color mud{0.55f, 0.38f, 0.20f};
    Color open{0.35f, 0.85f, 0.55f};
    Color visited{0.40f, 0.60f, 0.95f};
    Color deadEnd{0.85f, 0.35f, 0.35f};
    Color path{1.00f, 0.85f, 0.15f};
    Color start{0.10f, 0.75f, 0.30f};
    Color goal{0.90f, 0.20f, 0.25f};
    Color agent{0.95f, 0.30f, 0.85f};
};

inline Color mixColors(const Color& from, const Color& to, float t) {
    return {from[0] + (to[0] - from[0]) * t, from[1] + (to[1] - from[1]) * t, from[2] + (to[2] - from[2]) * t};
}
