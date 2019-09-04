#import <AudioToolbox/AudioServices.h>
#include <algorithm>
#include "GKSystemService.h"

namespace {

    AudioDeviceID defaultOutputDeviceID() {
        // get output device device
        AudioObjectPropertyAddress propertyAOPA;
        propertyAOPA.mScope = kAudioObjectPropertyScopeGlobal;
        propertyAOPA.mElement = kAudioObjectPropertyElementMaster;
        propertyAOPA.mSelector = kAudioHardwarePropertyDefaultOutputDevice;
	
        if (!AudioHardwareServiceHasProperty(kAudioObjectSystemObject, &propertyAOPA))
            return kAudioObjectUnknown;

        AudioDeviceID outputDeviceID = kAudioObjectUnknown;
        UInt32 propertySize = sizeof(AudioDeviceID);
        OSStatus status = AudioHardwareServiceGetPropertyData(kAudioObjectSystemObject, &propertyAOPA, 0, NULL, &propertySize, &outputDeviceID);
	
        if(status) 
            return kAudioObjectUnknown;
        else
            return outputDeviceID;
    }

    template <typename T>
        OSStatus setAudioProperty(AudioObjectPropertySelector property, T value) {
        AudioDeviceID outputDeviceID = defaultOutputDeviceID();
        if (outputDeviceID == kAudioObjectUnknown)
            return kAudioHardwareBadDeviceError;
        AudioObjectPropertyAddress propertyAOPA;
        propertyAOPA.mElement = kAudioObjectPropertyElementMaster;
        propertyAOPA.mScope = kAudioDevicePropertyScopeOutput;
        propertyAOPA.mSelector = property;
        if (!AudioHardwareServiceHasProperty(outputDeviceID, &propertyAOPA))
            return kAudioHardwareUnknownPropertyError;

        Boolean canSet = NO;
        OSStatus status = AudioHardwareServiceIsPropertySettable(outputDeviceID, &propertyAOPA, &canSet);
        if (status || !canSet)
            return kAudioHardwareUnsupportedOperationError;

        UInt32 propertySize = sizeof(value);
        return AudioHardwareServiceSetPropertyData(outputDeviceID, &propertyAOPA, 0, NULL, propertySize, &value);
    }

    template <typename T>
        OSStatus getAudioProperty(AudioObjectPropertySelector property, T & value) {
        AudioDeviceID outputDeviceID = defaultOutputDeviceID();
        if (outputDeviceID == kAudioObjectUnknown)
            return kAudioHardwareBadDeviceError;

        AudioObjectPropertyAddress propertyAOPA;
        propertyAOPA.mElement = kAudioObjectPropertyElementMaster;
        propertyAOPA.mSelector = property;
        propertyAOPA.mScope = kAudioDevicePropertyScopeOutput;
        if (!AudioHardwareServiceHasProperty(outputDeviceID, &propertyAOPA))
            return kAudioHardwareUnknownPropertyError;

        UInt32 propertySize = sizeof(value);
        return AudioHardwareServiceGetPropertyData(outputDeviceID, &propertyAOPA, 0, NULL, &propertySize, &value);
    }

    float getVolume() {
        Float32 outputVolume;
        OSStatus status = getAudioProperty(kAudioHardwareServiceDeviceProperty_VirtualMasterVolume, outputVolume);
        if (status)
            return 0.0;
        if (outputVolume < 0.0 || outputVolume > 1.0)
            return 0.0;
        return outputVolume;
    }

    void mute() {
        setAudioProperty<UInt32>(kAudioDevicePropertyMute, 1);
    }

    void unmute() {
        setAudioProperty<UInt32>(kAudioDevicePropertyMute, 0);
    }

    void setVolume(Float32 newVolume) {
        if (newVolume < 0.0 || newVolume > 1.0)
            return;
        
        if (newVolume < 0.001) {
            mute();
        }
        else {
            setAudioProperty<Float32>(kAudioHardwareServiceDeviceProperty_VirtualMasterVolume, newVolume);
            unmute();
        }
    }
}

namespace GKSystemService {
    void
        adjustVolume(short value) {
        float d = value / 100.0;
        float vol = getVolume();
        vol = std::clamp(vol + d, 0.f, 1.f);
        setVolume(vol);
    }

    void
        muteVolume() {
        mute();
    }

    void
        unmuteVolume() {
        unmute();
    }
}
