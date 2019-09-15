#include <codecvt>
#include <map>
#include <Windows.h>
#include <tlhelp32.h>
#include <Shlobj.h>
#include <tchar.h>
#include "GKWin.h"

#pragma comment(lib, "Shell32.lib")


namespace {
    // convert UTF-8 string to wstring
    std::wstring utf8ToWString(const std::string& str)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
        return myconv.from_bytes(str);
    }

    // convert wstring to UTF-8 string
    std::string wStringToUtf8(const std::wstring& str)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
        return myconv.to_bytes(str);
    }
}

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

    DWORD GetProcessByName(PCTCH name) {
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
                if (_tcscmp(process.szExeFile, name) == 0)
                {
                    pid = process.th32ProcessID;
                    break;
                }
            } while (Process32Next(snapshot, &process));
        }
        CloseHandle(snapshot);
        return pid;
    }

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

GKAppProxy::Imp::Imp(GKAppProxy* parent)
    : parent_(parent) {}


GKErr
GKAppProxy::Imp::bringFront() {
    update();
    if (!running())
        return GKErr::appNotRunning;

    ShowWindow(data_.windowHandle, SW_RESTORE);
    SetForegroundWindow(data_.windowHandle);
    return GKErr::noErr;
}

GKErr
GKAppProxy::Imp::show() {
    update();
    if (!running())
        return GKErr::appNotRunning;

    ShowWindow(data_.windowHandle, SW_RESTORE);
    return GKErr::noErr;
}

GKErr
GKAppProxy::Imp::hide() {
    update();
    if (!running())
        return GKErr::appNotRunning;
    ShowWindow(data_.windowHandle, SW_MINIMIZE);
    return GKErr::noErr;
}

bool
GKAppProxy::Imp::visible() const {
    update();
    if (!running())
        return false;
    return IsWindowVisible(data_.windowHandle);
}

bool
GKAppProxy::Imp::atFrontmost() const {
    update();

    if (!running())
        return false;

    DWORD threadId = GetWindowThreadProcessId(data_.windowHandle, NULL);
    if (threadId) {
        GUITHREADINFO gui;
        gui.cbSize = sizeof(GUITHREADINFO);
        if (GetGUIThreadInfo(threadId, &gui)) {
            return data_.windowHandle == gui.hwndActive || data_.windowHandle == gui.hwndFocus;
        }
    }
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
    data_.processId = Win32::GetProcessByName(utf8ToWString(parent_->descriptor_).c_str());
    if (data_.processId)
        data_.windowHandle = Win32::findMainWinow(data_.processId);
    else
        data_.windowHandle = 0;
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

void
GKSystemImp::postNotification(const std::string & title, const std::string & message) {
    ::MessageBoxA(NULL, message.c_str(), title.c_str(), MB_OK | MB_TOPMOST);
    // TODO:
}

std::string
GKSystemImp::applicationSupportFolder() {
    WCHAR szPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath)))
        return wStringToUtf8(szPath);
    else
        return std::string();
}

void
GKSystemImp::revealFile(const std::string& file) {
    auto fileW = utf8ToWString(file);
    PIDLIST_ABSOLUTE pidl;
    SHParseDisplayName(fileW.c_str(), nullptr, &pidl, 0, nullptr);
    SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);
    CoTaskMemFree(pidl);
}
