#include "GKAppConfig.h"

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
#if GK_WIN
    entries_.push_back({ "F1", "toggle WindowsTerminal.exe" });
    entries_.push_back({ "F2", "toggle devenv.exe" });
    entries_.push_back({ "CTRL+DOWN", "volume -5" });
    entries_.push_back({ "CTRL+UP", "volume +5" });
#else
#endif
}
