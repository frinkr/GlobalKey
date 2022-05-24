#pragma once
#include <map>
#include <string>

#include "GKHotkey.h"

struct GKConfig {
    struct KeyMapItem {
        std::string command;
        std::string menu;
    };
    using KeyMap = std::map<GKKeySequence, KeyMapItem>;
    
    bool autoRepeat {};
    KeyMap keyMap {};

    bool
    load(const std::string & path);

    bool
    save(const std::string & path) const;

    static const GKConfig &
    sample();
};
