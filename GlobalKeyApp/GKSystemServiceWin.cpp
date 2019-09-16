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
    class Volume {
    public:
        Volume() {
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

        ~Volume() {
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

        Volume vol;
        float currentVolume = 0;
        vol.ep()->GetMasterVolumeLevel(&currentVolume);
        
        float minDb, maxDb, stepDb;
        vol.ep()->GetVolumeRange(&minDb, &maxDb, &stepDb);
        
        float dbValue = (maxDb - minDb) * value / 100.0;
        currentVolume += dbValue;
        currentVolume = std::clamp(currentVolume, minDb, maxDb);
        
        vol.ep()->SetMasterVolumeLevel((float)currentVolume, NULL);

    }

    bool
    audioMuted() {
        BOOL m = FALSE;
        Volume vol;
        if (SUCCEEDED(vol.ep()->GetMute(&m)))
            return m;
        else
            return false;
    }
    
    void
    muteAudio() {
        Volume vol;
        vol.ep()->SetMute(TRUE, NULL);
    }

    void
    unmuteAudio() {
        Volume vol;
        vol.ep()->SetMute(FALSE, NULL);
    }

    void
    open(const std::string& path) {
        ShellExecuteW(0, 0, utf8ToWString(path).c_str(), 0, 0, SW_SHOW);
    }

    void
    openUrl(const std::string& url) {
        open(url);
    }
}


void
GKCoreApp::registerAutoRun() {
    auto path = appExePath();
    HKEY hkey = NULL;
    RegCreateKey(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey);
    RegSetValueEx(hkey, L"GlobalKey", 0, REG_SZ, (BYTE*)path.c_str(), (path.size() + 1) * 2);
    RegCloseKey(hkey);
}

void
GKCoreApp::unregisterAutoRun() {
    HKEY hkey = NULL;
    RegCreateKey(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey);
    RegDeleteValue(hkey, L"GlobalKey");
    RegCloseKey(hkey);
}

bool
GKCoreApp::isAutoRunRegistered() {
    TCHAR path[MAX_PATH + 1] = { 0 };
    DWORD len = MAX_PATH + 1;
    RegGetValue(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", L"GlobalKey", RRF_RT_REG_SZ, NULL, path, &len);
    return path == appExePath();
    return false;
}