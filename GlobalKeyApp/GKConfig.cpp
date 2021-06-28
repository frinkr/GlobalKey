#include <fstream>
#include <yaml-cpp/yaml.h>
#include "GK.h"
#include "GKConfig.h"


const GKConfig &
GKConfig::sample() {
    static GKConfig config = [] {
        GKConfig config;
        config.autoRepeat = true;
#if GK_WIN
        config.keys = {
            { "F1", "toggle WindowsTerminal.exe" },
            { "F2", "toggle devenv.exe" },
            { "CTRL+DOWN", "volume -5" },
            { "CTRL+UP", "volume +5" },

        };
#else
        config.keys = {
            { "F1", "toggle org.gnu.Emacs" },
            { "F2", "toggle /Applications/iTerm.app" },
            { "F3", "toggle /Applications/Google Chrome.app" },
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
            auto key = kv.first.as<std::string>();
            auto cmd = kv.second.as<std::string>();
            if (auto pos = cmd.find_last_not_of("\n\r"); pos != cmd.npos && (pos + 1) != cmd.size())
                cmd = cmd.substr(0, pos + 1);
            keys[key] = cmd;
        }
    }
    return true;
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
                out << Key << "keys" << keys;
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
