#include "GKSystemService.h"
#include "GKCoreApp.h"
#include <codecvt>
#include <algorithm>
#include <Windows.h>
#include <Mmdeviceapi.h>
#include <EndpointVolume.h>
#include <Shlobj.h>

#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "User32.lib")

namespace {
    // convert UTF-8 string to wstring
    std::wstring utf8ToWString(const std::string& str) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
        return myconv.from_bytes(str);
    }

    // convert wstring to UTF-8 string
    std::string wStringToUtf8(const std::wstring& str) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
        return myconv.to_bytes(str);
    }

    std::wstring appExePath()  {
        TCHAR szFileName[MAX_PATH + 1];
        GetModuleFileName(NULL, szFileName, MAX_PATH + 1);
        return szFileName;
    }
}

namespace GKSystemService {
    class Audio {
    public:
        Audio() {
            HRESULT hr;

            CoInitialize(NULL);
            IMMDeviceEnumerator* deviceEnumerator = NULL;
            hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), 
                                  NULL,
                                  CLSCTX_INPROC_SERVER, 
                                  __uuidof(IMMDeviceEnumerator), 
                                  (LPVOID*)& deviceEnumerator);

            IMMDevice* defaultDevice = NULL;
            hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
            deviceEnumerator->Release();
            
            hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume),
                                         CLSCTX_INPROC_SERVER,
                                         NULL,
                                         (LPVOID*)& endpointVolume);
            defaultDevice->Release();
        }

        ~Audio() {
            if (endpointVolume)
                endpointVolume->Release();
            CoUninitialize();
        }

        IAudioEndpointVolume* ep() {
            return endpointVolume;
        }

    private:
        IAudioEndpointVolume* endpointVolume {};
    };

    void
    adjustVolume(short value) {

        Audio audio;

        float currentVolume = 0;
        audio.ep()->GetMasterVolumeLevelScalar(&currentVolume);

        currentVolume = std::clamp<float>((currentVolume * 100 + value) / 100, 0, 1);
        audio.ep()->SetMasterVolumeLevelScalar(currentVolume, NULL);

        showMessage("Volumne ", int(std::round(currentVolume * 100)));
    }

    bool
    audioMuted() {
        BOOL m = FALSE;
        Audio audio;
        if (SUCCEEDED(audio.ep()->GetMute(&m)))
            return m;
        else
            return false;
    }
    
    void
    muteAudio() {
        Audio audio;
        audio.ep()->SetMute(TRUE, NULL);
    }

    void
    unmuteAudio() {
        Audio audio;
        audio.ep()->SetMute(FALSE, NULL);
    }

    void
    open(const std::string& path) {
        ShellExecuteW(0, 0, utf8ToWString(path).c_str(), 0, 0, SW_SHOW);
    }

    void
    openUrl(const std::string& url) {
        open(url);
    }

    void
    lockscreen() {
        LockWorkStation();
    }
    
    extern "C" void PostNotificationWinImp(LPCWSTR pTitle, LPCWSTR pMessage);

    void
    postNotification(const std::string & title, const std::string & message, const std::string & icon) {
        PostNotificationWinImp(utf8ToWString(title).c_str(), utf8ToWString(message).c_str());
    }

    std::string
    applicationSupportFolder() {
        WCHAR szPath[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath)))
            return wStringToUtf8(szPath);
        else
            return std::string();
    }

    void
    revealFile(const std::string& file) {
        auto fileW = utf8ToWString(file);
        PIDLIST_ABSOLUTE pidl;
        SHParseDisplayName(fileW.c_str(), nullptr, &pidl, 0, nullptr);
        SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);
        CoTaskMemFree(pidl);
    }

    void
    registerAutoRun() {
        auto path = appExePath();
        HKEY hkey = NULL;
        RegCreateKey(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey);
        RegSetValueEx(hkey, L"GlobalKey", 0, REG_SZ, (BYTE*)path.c_str(), (path.size() + 1) * 2);
        RegCloseKey(hkey);
    }

    void
    unregisterAutoRun() {
        HKEY hkey = NULL;
        RegCreateKey(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey);
        RegDeleteValue(hkey, L"GlobalKey");
        RegCloseKey(hkey);
    }

    bool
    isAutoRunRegistered() {
        TCHAR path[MAX_PATH + 1] = { 0 };
        DWORD len = MAX_PATH + 1;
        RegGetValue(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", L"GlobalKey", RRF_RT_REG_SZ, NULL, path, &len);
        return path == appExePath();
    }
}
