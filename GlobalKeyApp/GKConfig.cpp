#include <fstream>
#include <yaml-cpp/yaml.h>
#include "GK.h"
#include "GKConfig.h"

namespace {
    void trimString(std::string & str) {
        if (auto pos = str.find_last_not_of("\n\r"); pos != str.npos && (pos + 1) != str.size())
            str = str.substr(0, pos + 1);
    }
}

const GKConfig &
GKConfig::sample() {
    static GKConfig config = [] {
        GKConfig config;
        config.autoRepeat = true;
#if GK_WIN
        config.keyMap = {
            { "F1", {"toggle WindowsTerminal.exe"} },
            { "F2", {"toggle devenv.exe"} },
            { "CTRL+DOWN", {"volume -5"} },
            { "CTRL+UP", {"volume +5"} },

        };
#else
        config.keyMap = {
            { "F1", {"toggle org.gnu.Emacs"} },
            { "F2", {"toggle /Applications/iTerm.app"} },
            { "F3", {"toggle /Applications/Google Chrome.app"} },
        };
#endif
        return config;
    }();
    return config;
}

bool
GKConfig::load(const std::string & path) {
    auto root = YAML::LoadFile(path);
    
    if (auto node = root["config"]) {
        autoRepeat = node["autorepeat"].as<bool>();
    }
    if (auto node = root["keys"]) {
        for (auto kv : node) {
            std::string cmd, menu;
            auto key = kv.first.as<std::string>();
            if (kv.second.Type() == YAML::NodeType::Scalar)
                cmd = kv.second.as<std::string>();
            else if (kv.second.Type() == YAML::NodeType::Map) {
                cmd = kv.second["cmd"].as<std::string>();
                menu = kv.second["menu"].as<std::string>();
            }
            trimString(cmd);
            trimString(menu);
            
            keyMap[key] = KeyMapItem{cmd, menu};
        }
    }
    return true;
}

YAML::Emitter& operator << (YAML::Emitter& out, const GKConfig::KeyMapItem & e) {
    if (e.menu.empty()) {
        out << e.command;
    }
    else {
        out << YAML::BeginMap
            << YAML::Key << "name" << YAML::Value << e.menu
            << YAML::Key << "cmd" << YAML::Value << e.command
            << YAML::EndMap;
    }
    return out;
}
    

bool
GKConfig::save(const std::string & path) const {
    using namespace YAML;
    try {
        Emitter out;

        out << BeginDoc; {
            out << BeginMap; {
                out << Key << "config"
                    << BeginMap; {
                    out << Key << "autorepeat" << Value << autoRepeat;
                    out << EndMap;
                }
                out << Key << "keys" << keyMap;
                out << EndMap;
            }

            out << EndDoc;
        }

        std::ofstream of(path);
        of << out.c_str();

        return true;
    }
    catch (...) {
        return false;
    }
}
