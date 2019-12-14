#include <map>
#include "GKHotkeyWin.h"

namespace Win32 {
    
    std::pair<UINT, UINT>
    parseKeySequence(const GKKeySequence& commandKeySequence) {
        auto p = GKSplitKeySequence(commandKeySequence);
        if (!p)
            return { -1, -1 };

        auto [mod, key] = *p;

        UINT winMod = MOD_NOREPEAT;
        if (mod & kSHIFT) winMod |= MOD_SHIFT;
        if (mod & kALT) winMod |= MOD_ALT;
        if (mod & kCTRL) winMod |= MOD_CONTROL;
        if (mod & kMETA) winMod |= MOD_WIN;

        if (key.size() == 1) {
            const SHORT vKey = VkKeyScanA(key[0]);
            if (vKey > -1)
                return { winMod, vKey };
        }

        UINT winKey;
        static const std::map<std::string, UINT> map{
            {kF1, VK_F1},
            {kF2, VK_F2},
            {kF3, VK_F3},
            {kF4, VK_F4},
            {kF5, VK_F5},
            {kF6, VK_F6},
            {kF7, VK_F7},
            {kF8, VK_F8},
            {kF9, VK_F9},
            {kF10, VK_F10},
            {kF11, VK_F11},
            {kF12, VK_F12},
            {kF13, VK_F13},
            {kF14, VK_F14},
            {kF15, VK_F15},
            {kF16, VK_F16},
            {kF17, VK_F17},
            {kF18, VK_F18},

            {kF19, VK_F19},
            {kF20, VK_F20},
            {kF21, VK_F21},
            {kF22, VK_F22},
            {kF23, VK_F23},
            {kF24, VK_F24},
            {kSpace, VK_SPACE},
            {kLeft, VK_LEFT},
            {kRight, VK_RIGHT},
            {kDown, VK_DOWN},
            {kUp, VK_UP},
            {kPageDown, VK_NEXT},
            {kPageUp, VK_PRIOR},
        };

        if (auto itr = map.find(key); itr != map.end())
            return { winMod, itr->second };
        else
            return { -1, -1 };
    }
}

GKHotkey::Imp::Imp(GKHotkey * parent, HWND hwnd)
    : parent_{parent}
    , hwnd_(hwnd)
{
    std::tie(modifiers_, virtualKey_) = Win32::parseKeySequence(parent_->keySequence_);
}

GKErr
GKHotkey::Imp::registerHotkey()
{
    if (virtualKey_ == -1)
        return GKErr::hotkeySequenceNotValid;

    if (RegisterHotKey(hwnd_, reinterpret_cast<int>(ref()), modifiers_, virtualKey_))
        return GKErr::noErr;
    else
        return GKErr::hotkeyCantRegister;
}


GKErr
GKHotkey::Imp::unregisterHotkey()
{
    if (virtualKey_ == -1)
        return GKErr::hotkeySequenceNotValid;

    if (UnregisterHotKey(hwnd_, reinterpret_cast<int>(ref())))
        return GKErr::noErr;
    else
        return GKErr::hotkeyCantUnregisteer;
}

GKHotkey::Ref
GKHotkey::Imp::ref() const {
    return GKHotkey::Ref((modifiers_ << 16) + virtualKey_);
}

HWND GKHotkeyTargetHWND  = 0;


