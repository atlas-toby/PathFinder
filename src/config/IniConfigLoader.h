#pragma once
#include "config/AppConfig.h"
#include <istream>
#include <string>

// "key = value" lines; lines starting with '#' or ';' are comments.
// Returns defaults if the file is missing; throws ConfigError on a malformed line.
class IniConfigLoader {
public:
    AppConfig load(const std::string& path) const;
    AppConfig parse(std::istream& in, const std::string& sourceName) const;

private:
    void apply(AppConfig& config, const std::string& key, const std::string& value) const;
};
