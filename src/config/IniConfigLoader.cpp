#include "config/IniConfigLoader.h"
#include "config/ConfigError.h"
#include <fstream>
#include <functional>
#include <map>

namespace {
using Setter = std::function<void(AppConfig&, const std::string&)>;

std::string trim(const std::string& text) {
    size_t first = text.find_first_not_of(" \t\r");
    if (first == std::string::npos) return "";
    return text.substr(first, text.find_last_not_of(" \t\r") - first + 1);
}

Setter intField(int AppConfig::*field) { return [field](AppConfig& c, const std::string& v) { c.*field = std::stoi(v); }; }
Setter floatField(float AppConfig::*field) { return [field](AppConfig& c, const std::string& v) { c.*field = std::stof(v); }; }
Setter textField(std::string AppConfig::*field) { return [field](AppConfig& c, const std::string& v) { c.*field = v; }; }

const std::map<std::string, Setter>& setters() {
    static const std::map<std::string, Setter> table = {
        {"window_width", intField(&AppConfig::windowWidth)},     {"window_height", intField(&AppConfig::windowHeight)},
        {"panel_width", floatField(&AppConfig::panelWidth)},     {"font_path", textField(&AppConfig::fontPath)},
        {"font_size", floatField(&AppConfig::fontSize)},         {"grid_width", intField(&AppConfig::gridWidth)},
        {"grid_height", intField(&AppConfig::gridHeight)},       {"wall_density", floatField(&AppConfig::wallDensity)},
        {"mud_density", floatField(&AppConfig::mudDensity)},     {"mud_cost", floatField(&AppConfig::mudCost)},
        {"steps_per_second", floatField(&AppConfig::stepsPerSecond)},
        {"agent_speed", floatField(&AppConfig::agentSpeed)},     {"search_sound", textField(&AppConfig::searchSound)},
        {"found_sound", textField(&AppConfig::foundSound)},      {"map_file", textField(&AppConfig::mapFile)},
    };
    return table;
}
}  // namespace

AppConfig IniConfigLoader::load(const std::string& path) const {
    std::ifstream in(path);
    if (!in) return AppConfig{};
    return parse(in, path);
}

AppConfig IniConfigLoader::parse(std::istream& in, const std::string& sourceName) const {
    AppConfig config;
    std::string line;
    for (int number = 1; std::getline(in, line); number++) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;
        size_t equals = line.find('=');
        if (equals == std::string::npos) throw ConfigError(sourceName, number, "expected '='");
        try {
            apply(config, trim(line.substr(0, equals)), trim(line.substr(equals + 1)));
        } catch (const std::logic_error& error) {
            throw ConfigError(sourceName, number, error.what());
        }
    }
    return config;
}

void IniConfigLoader::apply(AppConfig& config, const std::string& key, const std::string& value) const {
    auto setter = setters().find(key);
    if (setter == setters().end()) throw std::invalid_argument("unknown key '" + key + "'");
    setter->second(config, value);
}
