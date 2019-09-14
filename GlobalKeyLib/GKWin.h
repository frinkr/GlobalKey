#pragma once

#include <Windows.h>
#include "GK.h"
#include "GKProxyApp.h"
#include "GKHotkey.h"
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

class GKHotkey::Imp {
public:
    explicit Imp(GKHotkey * parent, HWND hwnd);
    
    GKErr
    registerHotkey();
    
    GKErr
    unregisterHotkey();
    
    GKHotkey::Ref
    ref() const;

private:
    GKHotkey * parent_{};
    HWND hwnd_{};
    UINT modifiers_{};
    UINT virtualKey_{};
};

extern HWND GKHotkeyTargetHWND;

class GKSystemImp {
public:
    static void
    postNotification(const std::string & title, const std::string & message);

    static std::string
    applicationSupportFolder();

    static void
    revealFile(const std::string& file);
};
