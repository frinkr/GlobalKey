#include <filesystem>
#include <fstream>

#include "json.hpp"
#include "GKCommand.h"
#include "GKCoreApp.h"
#include "GKSystemService.h"
using json = nlohmann::json;

namespace {
    const json&
    sampleJson() {
#if GK_WIN
        static json j {
            {"config", {
                    {"autorepeat", "1"}
                }
            }, 
            {"keys", {
                    { "F1", "toggle WindowsTerminal.exe" },
                    { "F2", "toggle devenv.exe" },
                    { "CTRL+DOWN", "volume -5" },
                    { "CTRL+UP", "volume +5" },
                }
            }
        };
#else
        static json j{
            {"config", {
                    {"autorepeat", "1"}
                }  
            },
            {"keys", {
                    { "F1", "toggle org.gnu.Emacs" },
                    { "F2", "toggle /Applications/iTerm.app" },
                    { "F3", "toggle /Applications/Google Chrome.app" },
                },
            }
        };
#endif
        return j;
    }

    std::filesystem::path
    configFilePath() {
        std::filesystem::path appSupport(GKSystemService::applicationSupportFolder());
        auto dir = appSupport / GKAPP_NAME;
        if (!std::filesystem::exists(dir))
            std::filesystem::create_directories(dir);
        return dir / GKAPP_NAME ".json";
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
    GKSystemService::revealFile(configFile_);
}

const std::string&
GKCoreApp::configPath() const {
    return configFile_;
}

void
GKCoreApp::reload(bool autoRegister) {
    // Reload config
    commandEntries_.clear();
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
GKCoreApp::loadConfig() {
    auto p = configFilePath();
    configFile_ = p.u8string();

    try {
        json j;
        if (std::filesystem::exists(p)) {
            std::ifstream ifs(p);
            ifs >> j;
        }
        else {
            j = sampleJson();
            std::ofstream ofs(p);
            ofs << std::setw(4) << j << std::endl;
        }

        // Read 'config' section
        if (const json & cfg = j["config"]; !cfg.empty()) {
            autoRepeat_ = cfg["autorepeat"];
        }

        // Read 'keys' section
        const json & keys = j["keys"];
        for (auto& kv : keys.items())
            commandEntries_.push_back({ kv.key(), kv.value().get<std::string>() });
    } catch(...) {
        commandEntries_.clear();
        GKSystemService::showMessage("Failed to load config file, ", configFile_);
    }
}

void
GKCoreApp::createHotkeys() {
    for (auto& entry : commandEntries_) {
        GKPtr<GKHotkey> hotkey = std::make_shared<GKHotkey>(entry.commandKeySequence, autoRepeat_);
        hotkey->setHandler([commandText = entry.commandText]() {
            GKCommandEngine::instance().runCommand(commandText);
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
