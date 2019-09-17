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

class GKProxyApp::Imp {
public:
    explicit Imp(GKProxyApp* parent);

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
    GKProxyApp* parent_{};
};
