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
            {"F1", VK_F1},
            {"F2", VK_F2},
            {"F3", VK_F3},
            {"F4", VK_F4},
            {"F5", VK_F5},
            {"F6", VK_F6},
            {"F7", VK_F7},
            {"F8", VK_F8},
            {"F9", VK_F9},
            {"F10", VK_F10},
            {"F11", VK_F11},
            {"F12", VK_F12},
            {"F13", VK_F13},
            {"F14", VK_F14},
            {"F15", VK_F15},
            {"F16", VK_F16},
            {"F17", VK_F17},
            {"F18", VK_F18},
            {"F19", VK_F19},
            {"F20", VK_F20},
            {"F21", VK_F21},
            {"F22", VK_F22},
            {"F23", VK_F23},
            {"F24", VK_F24},
            {"SPACE", VK_SPACE},
            {"LEFT", VK_LEFT},
            {"RIGHT", VK_RIGHT},
            {"DOWN", VK_DOWN},
            {"UP", VK_UP},
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


