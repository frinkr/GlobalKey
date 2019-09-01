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
GKConfig::appCount() const {
    return entries_.size();
}

const std::string&
GKConfig::appKeySequence(size_t index) const {
    return entries_[index].keySequence;
}

const GKAppDescriptor&
GKConfig::appDescriptor(size_t index) const {
    return entries_[index].appDescriptor;
}

void
GKConfig::load() {
#if GK_WIN
    entries_.push_back({ "F1", "WindowsTerminal.exe" });
    entries_.push_back({ "F2", "devenv.exe" });
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
    for (size_t i = 0; i < GKConfig::instance().appCount(); ++i) {
        GKPtr<GKHotKey> hotKey = std::make_shared<GKHotKey>(GKConfig::instance().appKeySequence(i));
        hotKey->setHandler([i]() {
            GKToggleAppTask task(i);
            task.run();
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

