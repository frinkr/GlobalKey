#include "GKCommand.h"
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
    for (size_t i = 0; i < GKAppConfig::instance().commandCount(); ++i) {
        GKPtr<GKHotKey> hotKey = std::make_shared<GKHotKey>(GKAppConfig::instance().commandKeySequence(i));
        hotKey->setHandler([i]() {
            auto commandText = GKAppConfig::instance().commandText(i);
            GKCommandEngine::instance().runCommand(commandText);
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

