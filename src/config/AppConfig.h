#pragma once
#include <string>

// Read from config/app.ini; keys missing from the file keep these defaults.
struct AppConfig {
    int windowWidth = 1400;
    int windowHeight = 860;
    float panelWidth = 390.f;
    std::string fontPath = "C:/Windows/Fonts/segoeui.ttf";
    float fontSize = 18.f;

    int gridWidth = 40;
    int gridHeight = 30;
    float wallDensity = 0.30f;
    float mudDensity = 0.08f;
    float mudCost = 5.f;
    float stepsPerSecond = 60.f;
    float agentSpeed = 8.f;

    std::string searchSound = "sfx/search.wav";
    std::string foundSound = "sfx/found.wav";
    std::string mapFile = "map.txt";
};
