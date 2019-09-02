#pragma once

#include "GKHotKey.h"

class GKCoreApp : private GKNoCopy {
private:
    struct CommandEntry {
        GKKeySequence     commandKeySequence;
        std::string       commandText;
    };
public:
    static GKCoreApp&
    instance();
    
    virtual ~GKCoreApp();

    const std::string&
    configPath() const;

    void
    reload();

    void
    registerHotKeys();
    
    void
    unregisterHotKeys();
    
    void
    invokeHotKey(GKHotKey::Ref hotKeyRef);

private:
    void
    loadConfig();

    void
    createHotKeys();

private:
    std::vector<GKPtr<GKHotKey>> hotKeys_;
    std::string configFile_;
    std::vector<CommandEntry> entries_;
};

