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
        static json j{
            { "F1", "toggle /Applications/Emacs.app" },
            //{ "F2", "toggle devenv.exe" },
            //{ "CTRL+DOWN", "volume -5" },
            //{ "CTRL+UP", "volume +5" },
        };
#endif
        return j;
    }
}

const GKAppConfig&
GKAppConfig::instance() {
    static GKAppConfig config;
    return config;
}

GKAppConfig::GKAppConfig() {
    load();
}

const std::string &
GKAppConfig::path() const {
    return file_;
}

size_t
GKAppConfig::commandCount() const {
    return entries_.size();
}

const std::string&
GKAppConfig::commandKeySequence(size_t index) const {
    return entries_[index].commandKeySequence;
}

const std::string &
GKAppConfig::commandText(size_t index) const {
    return entries_[index].commandText;
}

void
GKAppConfig::load() {
    json j = sampleJson();
    for (auto& kv : j.items()) 
        entries_.push_back({ kv.key(), kv.value().get<std::string>() });
    
}
