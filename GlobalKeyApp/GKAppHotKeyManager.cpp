#include "GKTask.h"
#include "GKAppConfig.h"
#include "GKAppHotKeyManager.h"


GKAppHotKeyManager&
GKAppHotKeyManager::instance() {
    static GKAppHotKeyManager instance;
    return instance;
}

GKAppHotKeyManager::~GKAppHotKeyManager() = default;

void
GKAppHotKeyManager::loadHotKeys() {
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
GKAppHotKeyManager::registerHotKeys() {
    for (auto& hotKey : hotKeys_)
        hotKey->registerHotKey();
}

void
GKAppHotKeyManager::unregisterHotKeys() {
    for (auto& hotKey : hotKeys_)
        hotKey->unregisterHotKey();
}

const std::vector<GKPtr<GKHotKey>>&
GKAppHotKeyManager::hotKeys() const {
    return hotKeys_;
}

