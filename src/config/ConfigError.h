#pragma once
#include <stdexcept>
#include <string>

class ConfigError : public std::runtime_error {
public:
    ConfigError(const std::string& path, int line, const std::string& reason)
        : std::runtime_error("'" + path + "' line " + std::to_string(line) + ": " + reason) {}
};
