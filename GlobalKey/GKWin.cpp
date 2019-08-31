#include <map>
#include <Windows.h>
#include "GKWin.h"

GKPtr<GKApp>
GKWinAppFactory::getOrCreateApp(const GKAppDescriptor & appDescriptor) {
    return GKPtr<GKApp>();
}

GKWinHotKey::GKWinHotKey(HWND hwnd, const GKKeySequence & keySequence)
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


void
GKWinHotKeyManager::setHWND(HWND hwnd) {
    hwnd_ = hwnd;
}

HWND
GKWinHotKeyManager::hwnd() const {
    return hwnd_;
}

GKPtr<GKHotKey>
GKWinHotKeyManager::createHotKey(const std::string & keySequence) {
    return std::make_shared<GKWinHotKey>(hwnd(), keySequence);
}


void
GKWinConfig::load() {
    entries_.push_back({ "F1", "Notepad.exe" });
    entries_.push_back({ "F2", "ie.exe" });
}

void
GKWinSystem::postNotification(const std::string & title, const std::string & message) {
    // TODO:
}
