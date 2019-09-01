#include "GKSystemService.h"

#if GK_WIN
#include <Windows.h>
#include <Mmdeviceapi.h>
#include <EndpointVolume.h>

#pragma comment(lib, "Winmm.lib")
#endif

namespace GKSystemService {
    void
    adjustVolume(short value) {
#if GK_WIN

        HRESULT hr;

        CoInitialize(NULL);
        IMMDeviceEnumerator* deviceEnumerator = NULL;
        hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID*)& deviceEnumerator);
        IMMDevice* defaultDevice = NULL;

        hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
        deviceEnumerator->Release();
        deviceEnumerator = NULL;

        IAudioEndpointVolume* endpointVolume = NULL;
        hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID*)& endpointVolume);
        defaultDevice->Release();
        defaultDevice = NULL;


        float currentVolume = 0;
        endpointVolume->GetMasterVolumeLevel(&currentVolume);

        float minDb, maxDb, stepDb;
        endpointVolume->GetVolumeRange(&minDb, &maxDb, &stepDb);

        float dbValue = (maxDb - minDb) * value / 100.0;
        currentVolume += dbValue;

        if (currentVolume > maxDb) currentVolume = maxDb;
        if (currentVolume < minDb) currentVolume = minDb;
        
        hr = endpointVolume->SetMasterVolumeLevel((float)currentVolume, NULL);
       
       
        endpointVolume->Release();

        CoUninitialize();
#endif
    }
}
