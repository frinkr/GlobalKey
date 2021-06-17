// Headers
#include <vector>
#include <map>
#include <windows.h>
#include <tchar.h>
#include <commctrl.h>
#include "Resource.h"
#include <iostream>
#include <string>
#include "GK.h"
#include "GKHotKeyWin.h"
#include "..\GKCoreApp.h"
#include "..\GKSystemService.h"

// Libs
#pragma comment(lib, "comctl32.lib")

// Various consts & Defs
#define	WM_USER_SHELLICON WM_USER + 200
#define WM_TASKBAR_CREATE RegisterWindowMessage(_T("TaskbarCreated"))

#define NOTIF_TIMER_ID 50001

// Globals
HWND hWnd {};
HINSTANCE hInst {};
NOTIFYICONDATA structNID {};
UINT_PTR notifTimerHandle {};
std::wstring notifTitle;
std::wstring notifMessage;

void initApp(HWND hWnd) {
    GKHotkeyTargetHWND = hWnd;
    gkApp.reload(true);
}

BOOL OnHotkey(WPARAM wParam, LPARAM lParam) {
    gkApp.invokeHotkey(GKHotkey::Ref(wParam));
    return TRUE;
}

void PostNotification(LPCWSTR pTitle, LPCWSTR pMessage)
{
    NOTIFYICONDATA nid;
    memset(&nid, 0, sizeof(NOTIFYICONDATA));
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = IDI_TRAYICON;
    nid.uFlags = NIF_INFO;
    _tcscpy_s(nid.szInfo, sizeof(nid.szInfo), pMessage);
    _tcscpy_s(nid.szInfoTitle, sizeof(nid.szInfoTitle), pTitle);
    nid.dwInfoFlags = NIIF_INFO;
    nid.uTimeout = 2000;
    Shell_NotifyIcon(NIM_MODIFY, &nid);
}

void TryPostNotification()
{
    KillTimer(hWnd, notifTimerHandle);

    if (notifMessage.empty())
        return;

    PostNotification(L"", L""); // close existing ballon
    PostNotification(notifTitle.c_str(), notifMessage.c_str());

    notifMessage.clear();
    notifTitle.clear();
}

extern "C" void PostNotificationWinImp(LPCWSTR pTitle, LPCWSTR pMessage)
{
    notifTitle = pTitle;
    notifMessage = pMessage;
    notifTimerHandle = SetTimer(hWnd, NOTIF_TIMER_ID, 500, NULL);
}


/* ================================================================================================================== */

/*
  Name: ... WndProc(...)
  Desc: Main hidden "Window" that handles the messaging for our system tray
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    POINT lpClickPoint;
    
    switch (message)
    {
    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &structNID);	// Remove Tray Item
        PostQuitMessage(0);							// Quit
        break;
    case WM_USER_SHELLICON:			// sys tray icon Messages
        switch (LOWORD(lParam))
        {
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        {
            HMENU hMenu, hSubMenu;
            // get mouse cursor position x and y as lParam has the message itself
            GetCursorPos(&lpClickPoint);

            // Load menu resource
            hMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_POPUP_MENU));
            if (!hMenu)
                return -1;	// !0, message not successful?

            // Select the first submenu
            hSubMenu = GetSubMenu(hMenu, 0);
            if (!hSubMenu) {
                DestroyMenu(hMenu);        // Be sure to Destroy Menu Before Returning
                return -1;
            }

            // Set Enabled State
            if (gkApp.hotkeysRegistered())
                CheckMenuItem(hMenu, ID_POPUP_ENABLE, MF_BYCOMMAND | MF_CHECKED);
            else
                CheckMenuItem(hMenu, ID_POPUP_ENABLE, MF_BYCOMMAND | MF_UNCHECKED);

            if (gkApp.isAutoRunRegistered())
                CheckMenuItem(hMenu, ID_POPUP_AUTOSTART, MF_BYCOMMAND | MF_CHECKED);
            else
                CheckMenuItem(hMenu, ID_POPUP_AUTOSTART, MF_BYCOMMAND | MF_UNCHECKED);

            // Display menu
            SetForegroundWindow(hWnd);
            TrackPopupMenu(hSubMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, lpClickPoint.x, lpClickPoint.y, 0, hWnd, NULL);
            SendMessage(hWnd, WM_NULL, 0, 0);

            // Kill off objects we're done with
            DestroyMenu(hMenu);
        }
        break;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);	// Destroy Window
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_POPUP_EXIT:
            DestroyWindow(hWnd);		// Destroy Window
            break;
        case ID_POPUP_RELOAD:
            gkApp.reload(true);
            break;
        case ID_POPUP_EDIT:
            gkApp.revealConfigFile();
            break;
        case ID_POPUP_ABOUT:			// Open about box
            GKSystemService::showMessage("About");
            break;
        case ID_POPUP_ENABLE:			// Toggle Enable
            if (gkApp.hotkeysRegistered())
                gkApp.unregisterHotkeys();
            else
                gkApp.registerHotkeys();

            break;
        case ID_POPUP_AUTOSTART:
            if (gkApp.isAutoRunRegistered())
                gkApp.unregisterAutoRun();
            else
                gkApp.registerAutoRun();
        }
        break;
    case WM_HOTKEY:
        OnHotkey(wParam, lParam);
        break;
    case WM_TIMER:
        TryPostNotification();
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;		// Return 0 = message successfully proccessed
}

/*
  Name: ... WinMain(...)
  Desc: Main Entry point
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    WNDCLASSEX wc;
    HANDLE hMutexInstance;
    INITCOMMONCONTROLSEX iccex;

    // Check for single instance
    // ------------------------------
    // Note: I recommend to use the GUID Creation Tool for the most unique id
    // Tools->Create GUID for Visual Studio .Net 2003
    // Or search somewhere in the Platform SDK for other environments
    hMutexInstance = CreateMutex(NULL, FALSE, _T("TrayApp-{1EB489D6-6702-43cd-A859-C2BA7DB58B06}"));
    if (GetLastError() == ERROR_ALREADY_EXISTS || GetLastError() == ERROR_ACCESS_DENIED)
        return 0;

    // Copy instance so it can be used globally in other methods
    hInst = hInstance;

    // Init common controls (if you're using them)
    // ------------------------------
    // See: http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/common/structures/initcommoncontrolsex.asp
    iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    iccex.dwICC = ICC_UPDOWN_CLASS | ICC_LISTVIEW_CLASSES;
    if (!InitCommonControlsEx(&iccex)) {
        MessageBox(NULL, _T("Cannot Initialize Common Controls!"), _T("Error!"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Window "class"
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, (LPCTSTR)MAKEINTRESOURCE(IDI_TRAYICON));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = _T("GlobalKey Tray Application");
    wc.hIconSm = LoadIcon(hInstance, (LPCTSTR)MAKEINTRESOURCE(IDI_TRAYICON));
    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, _T("Window Registration Failed!"), _T("Error!"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Create the hidden window
    hWnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        _T("GlobalKey Tray Application"),
        _T("GlobalKey Tray Application Framework"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL);
    if (hWnd == NULL) {
        MessageBox(NULL, _T("Window Creation Failed!"), _T("Error!"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // tray icon settings
    memset(&structNID, 0, sizeof(NOTIFYICONDATA));
    structNID.cbSize = sizeof(NOTIFYICONDATA);
    structNID.hWnd = hWnd;
    structNID.uID = IDI_TRAYICON;
    structNID.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    structNID.uCallbackMessage = WM_USER_SHELLICON;
    structNID.hIcon = LoadIcon(hInst, (LPCTSTR)MAKEINTRESOURCE(IDI_TRAYICON));;
    _tcscpy_s(structNID.szTip, sizeof(structNID.szTip), L"GlobalKey");
    
    if (!Shell_NotifyIcon(NIM_ADD, &structNID)) {
        MessageBox(NULL, _T("Systray Icon Creation Failed!"), _T("Error!"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    initApp(hWnd);

    // message Loop
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
