#pragma once

#include "GKHotkey.h"
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
    registerHotkeys();
    
    void
    unregisterHotkeys();
    
    bool
    hotkeysRegistered() const;
    
    void
    invokeHotkey(GKHotkey::Ref hotkeyRef);
    
private:
    void
    loadConfig();

    void
    createHotkeys();

private:
    bool hotkeysEnabled_ {};
    std::vector<GKPtr<GKHotkey>> hotkeys_;
    std::string configFile_;
    std::vector<CommandEntry> entries_;
};

