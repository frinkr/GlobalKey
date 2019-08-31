#include <map>
#include <Windows.h>
#include <tlhelp32.h>

#include "GKWin.h"


namespace Win32 {
    using DWORD = DWORD;

    BOOL isMainWindow(HWND handle)
    {
        return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
    }

    BOOL CALLBACK enumWindowsCallback(HWND handle, LPARAM lParam)
    {
        auto & data = *(GKMainWindowData*)lParam;
        DWORD processId = 0;
        GetWindowThreadProcessId(handle, &processId);
        if (data.processId != processId || !isMainWindow(handle))
            return TRUE;
        data.windowHandle = handle;
        return FALSE;
    }

    HWND findMainWinow(DWORD processId)
    {
        GKMainWindowData data;
        data.processId = processId;
        data.windowHandle = 0;
        EnumWindows(enumWindowsCallback, (LPARAM)& data);
        return data.windowHandle;
    }

    DWORD GetProcessByName(PCSTR name) {
        DWORD pid = 0;

        // Create toolhelp snapshot.
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 process;
        ZeroMemory(&process, sizeof(process));
        process.dwSize = sizeof(process);

        // Walkthrough all processes.
        if (Process32First(snapshot, &process)) {
            do {
                // Compare process.szExeFile based on format of name, i.e., trim file path
                // trim .exe if necessary, etc.
                if (std::string(process.szExeFile) == std::string(name))
                {
                    pid = process.th32ProcessID;
                    break;
                }
            } while (Process32Next(snapshot, &process));
        }
        CloseHandle(snapshot);
        return pid;
    }
}

GKAppProxy::Imp::Imp(GKAppProxy* parent)
    : parent_(parent) {}


GKErr
GKAppProxy::Imp::bringFront() {
    update();
    SetForegroundWindow(data_.windowHandle);
    return GKErr::notImplemented;
}

GKErr
GKAppProxy::Imp::show() {
    update();
    return GKErr::notImplemented;
}

GKErr
GKAppProxy::Imp::hide() {
    update();
    return GKErr::notImplemented;
}

bool
GKAppProxy::Imp::visible() const {
    update();
    return false;
}

bool
GKAppProxy::Imp::atFrontmost() const {
    update();
    return false;
}

bool
GKAppProxy::Imp::running() const {
    update();
    return data_.processId != 0;
}

GKErr
GKAppProxy::Imp::launch() {
    update();
    return GKErr::notImplemented;
}

void
GKAppProxy::Imp::update() const {
    data_.processId = Win32::GetProcessByName(parent_->descriptor_.c_str());
    if (data_.processId)
        data_.windowHandle = Win32::findMainWinow(data_.processId);
    else
        data_.windowHandle = 0;
}


GKHotKey::Imp::Imp(GKHotKey * parent, HWND hwnd)
    : parent_{parent}
    , hwnd_(hwnd)
{
    modifiers_ = MOD_NOREPEAT;
    
    std::map<std::string, UINT> map {
        {"F1", VK_F1},
        {"F2", VK_F2},
        {"F3", VK_F3},
    };
    virtualKey_ = map[parent_->keySequence_];
}

void 
GKHotKey::Imp::registerHotKey()
{
    RegisterHotKey(hwnd_, id(), modifiers_, virtualKey_);
}

void 
GKHotKey::Imp::unregisterHotKey()
{
    UnregisterHotKey(hwnd_, id());
}

int 
GKHotKey::Imp::id() const {
    return (modifiers_ << 16) + virtualKey_;
}

HWND GKHotKeyCreationHWND  = 0;

void
GKWinConfig::load() {
    entries_.push_back({ "F1", "WindowsTerminal.exe" });
    entries_.push_back({ "F2", "devenv.exe" });
}

void
GKWinSystem::postNotification(const std::string & title, const std::string & message) {
    // TODO:
}
