#include <filesystem>
#include <fstream>

#include "GKConfig.h"
#include "GKCommand.h"
#include "GKCoreApp.h"
#include "GKSystemService.h"

namespace {
    std::filesystem::path
    configFilePath() {
        std::filesystem::path appSupport(GKSystemService::applicationSupportFolder());
        auto dir = appSupport / GKAPP_NAME;
        if (!std::filesystem::exists(dir))
            std::filesystem::create_directories(dir);
        return dir / GKAPP_NAME ".yaml";
    }
}

GKCoreApp&
GKCoreApp::instance() {
    static GKCoreApp instance;
    return instance;
}

GKCoreApp::~GKCoreApp() = default;

void
GKCoreApp::revealConfigFile() {
    GKSystemService::revealFile(configFilePath_);
}

const std::string&
GKCoreApp::configPath() const {
    return configFilePath_;
}

void
GKCoreApp::reload(bool autoRegister) {
    // Reload config
    loadConfig();

    // Reload hotkeys
    auto enabled = hotkeysEnabled_;
    unregisterHotkeys();
    hotkeys_.clear();
    createHotkeys();
    if (enabled || autoRegister)
        registerHotkeys();
}

void
GKCoreApp::registerHotkeys() {
    for (auto& hotkey : hotkeys_) {
        if (GKErr::noErr != hotkey->registerHotkey()) {
            GKSystemService::showMessage(hotkey->keySequence(), " can't be registered");
        }
    }
    hotkeysEnabled_ = true;
}

void
GKCoreApp::unregisterHotkeys() {
    for (auto& hotkey : hotkeys_) {
        if (GKErr::noErr != hotkey->unregisterHotkey())
            GKSystemService::showMessage(hotkey->keySequence(), " can't be unregistered");
    }
    hotkeysEnabled_ = false;
}

bool
GKCoreApp::hotkeysRegistered() const {
    return hotkeysEnabled_;
}

void
GKCoreApp::invokeHotkey(GKHotkey::Ref hotkeyRef) {
    for (auto& hotkey : hotkeys_) {
        if (hotkey->ref() == hotkeyRef) {
            hotkey->invoke();
            break;
        }
    }
}

void
GKCoreApp::runCommand(const std::string & command) {
    GKCommandEngine::instance().runCommand(command);
}
    
const GKConfig::KeyMap &
GKCoreApp::keyMap() const {
    return config_.keyMap;
}
    
void
GKCoreApp::loadConfig() {
    auto p = configFilePath();
    configFilePath_ = p.u8string();
    config_ = GKConfig();

    try {
        if (std::filesystem::exists(p)) {
            if (!config_.load(configFilePath_))
                throw std::runtime_error("bad config");
        }
        else {
            config_ = GKConfig::sample();
            config_.save(configFilePath_);
        }
    } catch(...) {
        GKSystemService::showMessage("Failed to load config file, ", configFilePath_);
    }
}

void
GKCoreApp::createHotkeys() {
    for (auto & [key, item] : config_.keyMap) {
        GKPtr<GKHotkey> hotkey = std::make_shared<GKHotkey>(key, config_.autoRepeat);
        auto & command = item.command; // Fix clang
        hotkey->setHandler([&command]() {
            GKCommandEngine::instance().runCommand(command);
            });
        hotkeys_.push_back(hotkey);
    }
}

#if GK_WIN
void
GKCoreApp::registerAutoRun() {
    GKSystemService::registerAutoRun();
}

void
GKCoreApp::unregisterAutoRun() {
    GKSystemService::unregisterAutoRun();
}

bool
GKCoreApp::isAutoRunRegistered() {
    return GKSystemService::isAutoRunRegistered();
}
#endif
