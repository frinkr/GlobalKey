#pragma once
#include <map>
#include <string>

struct GKConfig {
    bool autoRepeat {};
    std::map<std::string, std::string> keys {};

    bool
    load(const std::string & path);

    bool
    save(const std::string & path) const;

    static const GKConfig &
    sample();
};
