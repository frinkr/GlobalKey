#pragma once

#include "GKHotKey.h"
#include "GKSystem.h"

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
    revealConfigFile();

    void
    reload(bool autoRegister);

    void
    registerHotKeys();
    
    void
    unregisterHotKeys();
    
    bool
    hotKeysRegistered() const;
    
    void
    invokeHotKey(GKHotKey::Ref hotKeyRef);
    
private:
    void
    loadConfig();

    void
    createHotKeys();

private:
    bool hotKeysEnabled_ {};
    std::vector<GKPtr<GKHotKey>> hotKeys_;
    std::string configFile_;
    std::vector<CommandEntry> entries_;
};

