#include <map>
#include <Windows.h>
#include "GKWin.h"

GKWinHotKey::GKWinHotKey(HWND hwnd, const std::string & keySequence)
    : GKHotKey(keySequence)
    , hwnd_(hwnd)
{
    modifiers_ = MOD_NOREPEAT;

    std::map<std::string, UINT> map {
        {"F1", VK_F1},
        {"F2", VK_F2},
        {"F3", VK_F3},
    };
    virtualKey_ = map[keySequence];
}

void 
GKWinHotKey::registerHotKey()
{
    RegisterHotKey(hwnd_, id(), modifiers_, virtualKey_);
}

void 
GKWinHotKey::unregisterHotKey()
{
    UnregisterHotKey(hwnd_, id());
}

int 
GKWinHotKey::id() const {
    return (modifiers_ << 16) + virtualKey_;
}