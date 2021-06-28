#pragma once

#include "GKHotkey.h"
#include "GKConfig.h"

#define gkApp GKCoreApp::instance()
#define GKAPP_NAME "GlobalKey"
#define GKAPP_URL "https://github.com/frinkr/GlobalKey"

class GKCoreApp : private GKNoCopy {
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
    GKConfig config_ {};
    std::string configFilePath_;
    std::vector<GKPtr<GKHotkey>> hotkeys_;
};

