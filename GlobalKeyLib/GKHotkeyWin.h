#pragma once

#include <Windows.h>
#include "GKHotkey.h"

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
