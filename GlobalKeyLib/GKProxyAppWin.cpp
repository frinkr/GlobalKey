#include <codecvt>
#include <map>
#include <Windows.h>
#include <tlhelp32.h>
#include <Shlobj.h>
#include <tchar.h>
#include "GKProxyAppWin.h"

#pragma comment(lib, "Shell32.lib")

namespace {
    // convert UTF-8 string to wstring
    std::wstring utf8ToWString(const std::string& str)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
        return myconv.from_bytes(str);
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

}

GKProxyApp::Imp::Imp(GKProxyApp* parent)
    : parent_(parent) {}


GKErr
GKProxyApp::Imp::bringFront() {
    update();
    if (!running())
        return GKErr::appNotRunning;

    ShowWindow(data_.windowHandle, SW_RESTORE);
    SetForegroundWindow(data_.windowHandle);
    return GKErr::noErr;
}

GKErr
GKProxyApp::Imp::show() {
    update();
    if (!running())
        return GKErr::appNotRunning;

    ShowWindow(data_.windowHandle, SW_RESTORE);
    return GKErr::noErr;
}

GKErr
GKProxyApp::Imp::hide() {
    update();
    if (!running())
        return GKErr::appNotRunning;
    ShowWindow(data_.windowHandle, SW_MINIMIZE);
    return GKErr::noErr;
}

bool
GKProxyApp::Imp::visible() const {
    update();
    if (!running())
        return false;
    return IsWindowVisible(data_.windowHandle);
}

bool
GKProxyApp::Imp::atFrontmost() const {
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
GKProxyApp::Imp::running() const {
    update();
    return data_.processId != 0;
}

GKErr
GKProxyApp::Imp::launch() {
    update();
    return GKErr::notImplemented;
}

void
GKProxyApp::Imp::update() const {
    data_.processId = Win32::GetProcessByName(utf8ToWString(parent_->descriptor_).c_str());
    if (data_.processId)
        data_.windowHandle = Win32::findMainWinow(data_.processId);
    else
        data_.windowHandle = 0;
}

