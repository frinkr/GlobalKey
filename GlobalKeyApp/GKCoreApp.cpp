#include "json.hpp"
#include "GKCommand.h"
#include "GKCoreApp.h"

using json = nlohmann::json;

namespace {
    const json&
    sampleJson() {
#if GK_WIN
        static json j {
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

GKCoreApp&
GKCoreApp::instance() {
    static GKCoreApp instance;
    return instance;
}

GKCoreApp::~GKCoreApp() = default;

const std::string&
GKCoreApp::configPath() const {
    return configFile_;
}

void
GKCoreApp::reload() {
    // Reload config
    entries_.clear();
    loadConfig();

    // Reload hotkeys
    unregisterHotKeys();
    hotKeys_.clear();
    createHotKeys();
    registerHotKeys();
}

void
GKCoreApp::registerHotKeys() {
    for (auto& hotKey : hotKeys_)
        hotKey->registerHotKey();
}

void
GKCoreApp::unregisterHotKeys() {
    for (auto& hotKey : hotKeys_)
        hotKey->unregisterHotKey();
}

void
GKCoreApp::invokeHotKey(GKHotKey::Ref hotKeyRef) {
    for (auto& hotKey : hotKeys_) {
        if (hotKey->ref() == hotKeyRef) {
            hotKey->invoke();
            break;
        }
    }
}

void
GKCoreApp::loadConfig() {
    json j = sampleJson();
    for (auto& kv : j.items())
        entries_.push_back({ kv.key(), kv.value().get<std::string>() });
}

void
GKCoreApp::createHotKeys() {
    for (auto& entry : entries_) {
        GKPtr<GKHotKey> hotKey = std::make_shared<GKHotKey>(entry.commandKeySequence);
        hotKey->setHandler([commandText = entry.commandText]() {
            GKCommandEngine::instance().runCommand(commandText);
            });
        hotKeys_.push_back(hotKey);
    }
}
