#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <windows.h>
#include <tchar.h>
#include <gdiplus.h>

#include "GK.h"
#include "GKHotKeyWin.h"
#include "Resource.h"
#include "..\GKCoreApp.h"
#include "..\GKSystemService.h"


#pragma comment (lib,"Gdiplus.lib")

#define	WM_USER_SHELLICON WM_USER + 200
#define HIDE_NOTIFICATION_TIMER_ID 50001

// Globals
HWND s_hMainWnd {};
HWND s_hNotifWnd {};
NOTIFYICONDATA s_trayIconData {};
UINT_PTR s_hHideNotifWndTimer {};
UINT s_uTaskbarRestart{};
std::wstring s_sNotifTitle;
std::wstring s_sNotifMessage;
std::wstring s_sNotifIconName;
constexpr int s_uNotifWndWidth {300};
constexpr int s_uNotifWndHeight {100};

void initApp(HWND hWnd) {
    GKHotkeyTargetHWND = hWnd;
    gkApp.reload(true);
}

BOOL OnHotkey(WPARAM wParam, LPARAM lParam) {
    gkApp.invokeHotkey(GKHotkey::Ref(wParam));
    return TRUE;
}

int ShowError(LPCTSTR pMessage, int iError = -1) {
    MessageBox(NULL, pMessage, _T("Error"), MB_ICONEXCLAMATION | MB_OK);
    return iError;
}

void GetPreferredNotificationWindowPosition(int * x, int * y) {
    RECT rect;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

    *x = rect.left + (rect.right - rect.left - s_uNotifWndWidth) / 2;
    *y = rect.bottom - s_uNotifWndHeight * 1.0;
}

extern "C" void PostNotificationWinImp(LPCWSTR pTitle, LPCWSTR pMessage, LPCWSTR pIconName) {
    s_sNotifTitle = pTitle;
    s_sNotifMessage = pMessage;
    s_sNotifIconName = pIconName;
    
    s_hHideNotifWndTimer = SetTimer(s_hMainWnd, HIDE_NOTIFICATION_TIMER_ID, 1000, NULL);

    int x, y;
    GetPreferredNotificationWindowPosition(&x, &y);

    MoveWindow(s_hNotifWnd, x, y, s_uNotifWndWidth, s_uNotifWndHeight, FALSE);
    ShowWindow(s_hNotifWnd, SW_SHOWNORMAL);
    UpdateWindow(s_hNotifWnd);
    InvalidateRect(s_hNotifWnd, 0, TRUE);
}

int AddTrayIcon() {
    memset(&s_trayIconData, 0, sizeof(NOTIFYICONDATA));
    s_trayIconData.cbSize = sizeof(NOTIFYICONDATA);
    s_trayIconData.hWnd = s_hMainWnd;
    s_trayIconData.uID = IDI_TRAYICON;
    s_trayIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    s_trayIconData.uCallbackMessage = WM_USER_SHELLICON;
    s_trayIconData.hIcon = LoadIcon(GetModuleHandle(nullptr), (LPCTSTR)MAKEINTRESOURCE(IDI_TRAYICON));;
    _tcscpy_s(s_trayIconData.szTip, sizeof(s_trayIconData.szTip) / sizeof(s_trayIconData.szTip[0]), L"GlobalKey");
    
    if (!Shell_NotifyIcon(NIM_ADD, &s_trayIconData))
        return ShowError(_T("Systray Icon Creation Failed!"));
    
    return 0;
}

IStream * LoadStreamFromResource(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType) {
    IStream* pStream = nullptr;
    HGLOBAL hGlobal = nullptr;

    if (HRSRC hrsrc = FindResource(hModule, lpName, lpType)) {
        if (DWORD dwResourceSize = SizeofResource(hModule, hrsrc); dwResourceSize > 0) {
            if (HGLOBAL hGlobalResource = LoadResource(hModule, hrsrc)) {
                void* bytes = LockResource(hGlobalResource);
                if (hGlobal = ::GlobalAlloc(GHND, dwResourceSize)) {
                    if (void* pBuffer = ::GlobalLock(hGlobal)) {
                        memcpy(pBuffer, bytes, dwResourceSize);
                        if (SUCCEEDED(CreateStreamOnHGlobal(hGlobal, TRUE, &pStream)))
                            hGlobal = nullptr;
                        
                    }
                }
            }
        }
    }

    if (hGlobal) {
        GlobalFree(hGlobal);
        hGlobal = nullptr;
    }

    return pStream;
}


Gdiplus::Bitmap* LoadImageFromResource(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType) {
    if (IStream* stream = LoadStreamFromResource(hModule, lpName, lpType)) {
        Gdiplus::Bitmap* bm = new Gdiplus::Bitmap(stream);
        stream->Release();
        return bm;
    }
    return nullptr;
}

LRESULT CALLBACK WndProcMain(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
    POINT lpClickPoint;
    
    switch (uMessage)
    {
    case WM_CREATE:
        s_uTaskbarRestart = RegisterWindowMessage(_T("TaskbarCreated"));
        break;
        
    case WM_DESTROY:
        PostQuitMessage(0);
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
            hMenu = LoadMenu(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDR_POPUP_MENU));
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
        ShowWindow(s_hNotifWnd, SW_HIDE);
        break;
    default:
        if (uMessage == s_uTaskbarRestart)
            ;// return AddTrayIcon();
        break;
    }
    return DefWindowProc(hWnd, uMessage, wParam, lParam);
}

LPCTSTR MapResource(LPCTSTR lpName) {
    if (_tcsicmp(lpName, _T("vol")) == 0)
        return MAKEINTRESOURCE(IDB_PNG_VOL);
    return nullptr;
}

LRESULT CALLBACK WndProcNotification(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
    using namespace Gdiplus;

    switch (uMessage) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        Graphics g(hdc);

        float bmDestHeight = 0;
        float bmDestWidth = 0;
        float bmDestY = 0;
        float bmDestX = 0;

        if (auto bm = LoadImageFromResource(NULL, MapResource(s_sNotifIconName.c_str()), _T("PNG"))) {
            bmDestHeight = s_uNotifWndHeight * 0.6;
            bmDestWidth = bmDestHeight * bm->GetWidth() / bm->GetHeight();
            bmDestY = (s_uNotifWndHeight - bmDestHeight) / 2;
            bmDestX = bmDestY;
            g.DrawImage(bm, Rect(bmDestX, bmDestY, bmDestWidth, bmDestHeight));
            delete bm;
        }

        SolidBrush  brush(Color(255, 255, 255, 255));
        FontFamily  fontFamily(L"Arial");
        Font        font(&fontFamily, 24, FontStyleRegular, UnitPixel);

        StringFormat fmt;
        fmt.SetAlignment(StringAlignment::StringAlignmentCenter);
        fmt.SetLineAlignment(StringAlignment::StringAlignmentCenter);

        RectF rect(bmDestX + bmDestWidth, 0, s_uNotifWndWidth - bmDestWidth - 3 * bmDestX, s_uNotifWndHeight);
        g.DrawString(s_sNotifMessage.c_str(), -1, &font, rect, &fmt, &brush);

        EndPaint(hWnd, &ps);
    }
        break;
//    case WM_ERASEBKGND:
//        return TRUE;
    default:
        return DefWindowProc(hWnd, uMessage, wParam, lParam);
        break;
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    HANDLE hMutexInstance = CreateMutex(NULL, FALSE, _T("GlobalKey-{1EB489D6-6702-43cd-A859-C2BA7DB58B06}"));
    if (GetLastError() == ERROR_ALREADY_EXISTS || GetLastError() == ERROR_ACCESS_DENIED)
        return 0;

    // Window "class"
    WNDCLASSEX mainWndCls;
    mainWndCls.cbSize = sizeof(WNDCLASSEX);
    mainWndCls.style = CS_HREDRAW | CS_VREDRAW;
    mainWndCls.lpfnWndProc = WndProcMain;
    mainWndCls.cbClsExtra = 0;
    mainWndCls.cbWndExtra = 0;
    mainWndCls.hInstance = hInstance;
    mainWndCls.hIcon = LoadIcon(hInstance, (LPCTSTR)MAKEINTRESOURCE(IDI_TRAYICON));
    mainWndCls.hCursor = LoadCursor(NULL, IDC_ARROW);
    mainWndCls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    mainWndCls.lpszMenuName = NULL;
    mainWndCls.lpszClassName = _T("GlobalKey Tray Window Class");
    mainWndCls.hIconSm = LoadIcon(hInstance, (LPCTSTR)MAKEINTRESOURCE(IDI_TRAYICON));
    if (!RegisterClassEx(&mainWndCls)) 
        return ShowError(_T("Window Registration Failed!"));

    // Create the hidden window
    s_hMainWnd = CreateWindowEx(WS_EX_CLIENTEDGE,
                                _T("GlobalKey Tray Window Class"),
                                _T("GlobalKey Tray Window"),
                                WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT,
                                CW_USEDEFAULT,
                                CW_USEDEFAULT,
                                CW_USEDEFAULT,
                                NULL,
                                NULL,
                                hInstance,
                                NULL);
    if (s_hMainWnd == NULL)
        return ShowError(_T("Window Creation Failed!"));

    // Notification Window Class
    WNDCLASSEX notifWndCls;
    notifWndCls.cbSize = sizeof(WNDCLASSEX);
    notifWndCls.style = CS_HREDRAW | CS_VREDRAW;
    notifWndCls.lpfnWndProc = WndProcNotification;
    notifWndCls.cbClsExtra = 0;
    notifWndCls.cbWndExtra = 0;
    notifWndCls.hInstance = hInstance;
    notifWndCls.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    notifWndCls.hCursor = LoadCursor(NULL, IDC_ARROW);
    notifWndCls.hbrBackground = CreateSolidBrush(RGB(0x11, 0x11, 0x11));
    notifWndCls.lpszMenuName = NULL;
    notifWndCls.lpszClassName = _T("GlobalKey Notification Window Class");
    notifWndCls.hIconSm = LoadIcon(notifWndCls.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

    if (!RegisterClassEx(&notifWndCls))
        return ShowError(_T("Call to RegisterClassEx failed!"));

    // Notification Window
    s_hNotifWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_LAYERED,
                                 _T("GlobalKey Notification Window Class"),
                                 _T("GlobalKey Notification Window"),
                                 WS_POPUP,
                                 200, 200,
                                 s_uNotifWndWidth, s_uNotifWndHeight,
                                 NULL,
                                 NULL,
                                 hInstance,
                                 NULL
        );

    if (!s_hNotifWnd)
        return ShowError(_T("Call to CreateWindow failed!"));
    SetLayeredWindowAttributes(s_hNotifWnd, 0, 192, LWA_ALPHA);

    // Tray Icon
    AddTrayIcon();
    
    initApp(s_hMainWnd);

    // Init GDI+
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // message Loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);
    return 0;
}
