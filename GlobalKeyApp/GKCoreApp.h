#pragma once

#include "GKHotkey.h"

#define gkApp GKCoreApp::instance()
#define GKAPP_NAME "GlobalKey"

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
    
#if GK_WIN
    void
    registerAutoRun();

    void
    unregisterAutoRun();

    bool
    isAutoRunRegistered();
#endif

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

