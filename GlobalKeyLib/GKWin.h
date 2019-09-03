#pragma once

#include <Windows.h>
#include "GK.h"
#include "GKProxyApp.h"
#include "GKHotKey.h"
#include "GKSystem.h"

struct GKMainWindowData {
    DWORD processId;
    HWND  windowHandle;
};

class GKAppProxy::Imp {
public:
    explicit Imp(GKAppProxy* parent);

    GKErr
    bringFront();

    GKErr
    show();

    GKErr
    hide();

    bool
    visible() const;

    bool
    atFrontmost() const;

    bool
    running() const;

    GKErr
    launch();

private:
    void
    update() const;

private:
    mutable GKMainWindowData data_{};
    GKAppProxy* parent_{};
};

class GKHotKey::Imp {
public:
    explicit Imp(GKHotKey * parent, HWND hwnd);
    
    GKErr
    registerHotKey();
    
    GKErr
    unregisterHotKey();
    
    GKHotKey::Ref
    ref() const;

private:
    GKHotKey * parent_{};
    HWND hwnd_{};
    UINT modifiers_{};
    UINT virtualKey_{};
};

extern HWND GKHotKeyTargetHWND;

class GKSystemImp {
public:
    static void
    postNotification(const std::string & title, const std::string & message);

    static std::string
    applicationSupportFolder();

    static void
    revealFile(const std::string& file);
};
