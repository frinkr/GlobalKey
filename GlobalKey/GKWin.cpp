#include <map>
#include <Windows.h>
#include <tlhelp32.h>

#include "GKWin.h"


namespace Win32 {
    using ProcessId = DWORD;

    struct MainWindowData {
        ProcessId processId;
        HWND windowHandle;
    };

    BOOL isMainWindow(HWND handle)
    {
        return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
    }

    BOOL CALLBACK enumWindowsCallback(HWND handle, LPARAM lParam)
    {
        MainWindowData& data = *(MainWindowData*)lParam;
        ProcessId processId = 0;
        GetWindowThreadProcessId(handle, &processId);
        if (data.processId != processId || !isMainWindow(handle))
            return TRUE;
        data.windowHandle = handle;
        return FALSE;
    }

    HWND findMainWinow(ProcessId processId)
    {
        MainWindowData data;
        data.processId = processId;
        data.windowHandle = 0;
        EnumWindows(enumWindowsCallback, (LPARAM)& data);
        return data.windowHandle;
    }

    ProcessId GetProcessByName(PCSTR name) {
        ProcessId pid = 0;

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

struct GKWinApp::Imp {

    explicit Imp(GKWinApp* parent) : parent_(parent) {}

    void update() {
        data_.processId = Win32::GetProcessByName(parent_->descriptor_.c_str());
        if (data_.processId)
            data_.windowHandle = Win32::findMainWinow(data_.processId);
        else
            data_.windowHandle = 0;
    }

    Win32::MainWindowData data_;
    GKWinApp* parent_{};
};

GKWinApp::GKWinApp(const GKAppDescriptor & descriptor)
    : GKApp(descriptor)
    , imp_(std::make_unique<Imp>(this))
{
    imp_->update();
}

GKWinApp::~GKWinApp() {
    
}

GKErr
GKWinApp::bringFront() {
    SetForegroundWindow(imp_->data_.windowHandle);
    return GKErr::notImplemented;
}

GKErr
GKWinApp::show() {
    return GKErr::notImplemented;
}

GKErr
GKWinApp::hide() {
    return GKErr::notImplemented;
}

bool
GKWinApp::visible() const {
    return false;
}

bool
GKWinApp::atFrontmost() const {
    return false;
}

bool
GKWinApp::running() const {
    return imp_->data_.processId != 0;
}

GKErr
GKWinApp::launch() {
    return GKErr::notImplemented;
}

GKPtr<GKApp>
GKWinAppFactory::getOrCreateApp(const GKAppDescriptor & appDescriptor) {
    return std::make_shared<GKWinApp>(appDescriptor);
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
