#include "GKSystemService.h"

#if GK_WIN
#include <Windows.h>
#include <Mmdeviceapi.h>
#include <EndpointVolume.h>

#pragma comment(lib, "Winmm.lib")
#endif


namespace {
#if GK_WIN
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
#endif
}
namespace GKSystemService {
    void
    adjustVolume(short value) {
#if GK_WIN
        Volume vol;
        float currentVolume = 0;
        vol.ep()->GetMasterVolumeLevel(&currentVolume);
        
        float minDb, maxDb, stepDb;
        vol.ep()->GetVolumeRange(&minDb, &maxDb, &stepDb);
        
        float dbValue = (maxDb - minDb) * value / 100.0;
        currentVolume += dbValue;
        
        if (currentVolume > maxDb) currentVolume = maxDb;
        if (currentVolume < minDb) currentVolume = minDb;
        
        vol.ep()->SetMasterVolumeLevel((float)currentVolume, NULL);
#endif
    }

    void
    muteVolume() {
        Volume vol;
        vol.ep()->SetMute(TRUE, NULL);
    }

    void
    unmuteVolume() {
        Volume vol;
        vol.ep()->SetMute(FALSE, NULL);
    }
}
