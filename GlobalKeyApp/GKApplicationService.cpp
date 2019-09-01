#include "GKTask.h"
#include "GKApplicationService.h"

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


GKHotKeyManager&
GKHotKeyManager::instance() {
    static GKHotKeyManager instance;
    return instance;
}

GKHotKeyManager::~GKHotKeyManager() = default;

void
GKHotKeyManager::loadHotKeys() {
    for (size_t i = 0; i < GKConfig::instance().taskCount(); ++i) {
        GKPtr<GKHotKey> hotKey = std::make_shared<GKHotKey>(GKConfig::instance().taskKeySequence(i));
        hotKey->setHandler([i]() {
            auto taskCommand = GKConfig::instance().taskCommand(i);
            GKTaskEngine::instance().runTask(taskCommand);
        });
        hotKeys_.push_back(hotKey);
    }
}

void
GKHotKeyManager::registerHotKeys() {
    for (auto& hotKey : hotKeys_)
        hotKey->registerHotKey();
}

void
GKHotKeyManager::unregisterHotKeys() {
    for (auto& hotKey : hotKeys_)
        hotKey->unregisterHotKey();
}

const std::vector<GKPtr<GKHotKey>>&
GKHotKeyManager::hotKeys() const {
    return hotKeys_;
}

