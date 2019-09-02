#include "json.hpp"
#include <iostream>
#include "GKAppConfig.h"

using json = nlohmann::json;

namespace {
    const json &
    sampleJson() {
#if GK_WIN
        static json j{
            { "F1", "toggle WindowsTerminal.exe" },
            { "F2", "toggle devenv.exe" },
            { "CTRL+DOWN", "volume -5" },
            { "CTRL+UP", "volume +5" },
        };
#else
#endif
        return j;
    }
}

const GKConfig&
GKConfig::instance() {
    static GKConfig config;
    return config;
}

GKConfig::GKConfig() {
    load();
}

const std::string &
GKConfig::path() const {
    return file_;
}

size_t
GKConfig::taskCount() const {
    return entries_.size();
}

const std::string&
GKConfig::taskKeySequence(size_t index) const {
    return entries_[index].taskKeySequence;
}

const std::string &
GKConfig::taskCommand(size_t index) const {
    return entries_[index].taskCommand;
}

void
GKConfig::load() {
    json j = sampleJson();
    for (auto& kv : j.items()) 
        entries_.push_back({ kv.key(), kv.value().get<std::string>() });
    
}
