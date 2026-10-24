#pragma once
#include <stdexcept>
#include <string>

// Thrown when a map file cannot be opened or its format is invalid.
class MapFormatError : public std::runtime_error {
public:
    MapFormatError(const std::string& path, const std::string& reason)
        : std::runtime_error("'" + path + "': " + reason) {}
};
