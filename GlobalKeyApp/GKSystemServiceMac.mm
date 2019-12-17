#import <AppKit/AppKit.h>
#import <AudioToolbox/AudioServices.h>
#include <cmath>
#include <algorithm>
#include "GKSystemService.h"
#import "macOS/AppDelegate.h"

namespace {
    NSString *
    fromStdString(const std::string & str) {
        return [NSString stringWithCString:str.c_str()
                                  encoding:NSUTF8StringEncoding];
    }

    std::string
    toStdString(NSString * str) {
        return std::string([str UTF8String]);
    }
}

namespace Audio {
    AudioDeviceID defaultOutputDeviceID() {
        // get output device device
        AudioObjectPropertyAddress address;
        address.mSelector = kAudioHardwarePropertyDefaultOutputDevice;
        address.mScope = kAudioObjectPropertyScopeGlobal;
        address.mElement = kAudioObjectPropertyElementMaster;
	
        if (!AudioObjectHasProperty(kAudioObjectSystemObject, &address))
            return kAudioObjectUnknown;

        AudioDeviceID outputDeviceID = kAudioObjectUnknown;
        UInt32 propertySize = sizeof(AudioDeviceID);
        OSStatus status = AudioObjectGetPropertyData(kAudioObjectSystemObject, &address, 0, NULL, &propertySize, &outputDeviceID);
	
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
        AudioObjectPropertyAddress address;
        address.mScope = kAudioDevicePropertyScopeOutput;
        address.mSelector = property;
        address.mElement = kAudioObjectPropertyElementMaster;
        if (!AudioObjectHasProperty(outputDeviceID, &address))
            return kAudioHardwareUnknownPropertyError;

        Boolean canSet = NO;
        OSStatus status = AudioObjectIsPropertySettable(outputDeviceID, &address, &canSet);
        if (status || !canSet)
            return kAudioHardwareUnsupportedOperationError;

        UInt32 propertySize = sizeof(value);
        return AudioObjectSetPropertyData(outputDeviceID, &address, 0, NULL, propertySize, &value);
    }

    template <typename T>
        OSStatus getAudioProperty(AudioObjectPropertySelector property, T & value) {
        AudioDeviceID outputDeviceID = defaultOutputDeviceID();
        if (outputDeviceID == kAudioObjectUnknown)
            return kAudioHardwareBadDeviceError;

        AudioObjectPropertyAddress address;
        address.mSelector = property;
        address.mScope = kAudioDevicePropertyScopeOutput;
        address.mElement = kAudioObjectPropertyElementMaster;
        if (!AudioObjectHasProperty(outputDeviceID, &address))
            return kAudioHardwareUnknownPropertyError;

        UInt32 propertySize = sizeof(value);
        return AudioObjectGetPropertyData(outputDeviceID, &address, 0, NULL, &propertySize, &value);
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

    bool isMuted() {
        UInt32 muted = 0;
        if (getAudioProperty(kAudioDevicePropertyMute, muted))
            return false;
        return muted;
    }

    void mute() {
        if (isMuted())
            return;
        setAudioProperty<UInt32>(kAudioDevicePropertyMute, 1);
    }

    void unmute() {
        if (isMuted())
            setAudioProperty<UInt32>(kAudioDevicePropertyMute, 0);
    }

    void setVolume(Float32 newVolume) {
        if (newVolume < 0.0 || newVolume > 1.0)
            return;
        setAudioProperty<Float32>(kAudioHardwareServiceDeviceProperty_VirtualMasterVolume, newVolume);
        if (newVolume < 0.001)
            mute();
        else
            unmute();
    }
}

namespace GKSystemService {
    void
    adjustVolume(short value) {
        short vol = std::roundf(Audio::getVolume() * 100);
        vol = std::clamp(vol + value, 0, 100);
        Audio::setVolume(vol / 100.0);
        
        postNotificationImp("Volume", "Volume " + std::to_string(vol), "vol");
    }

    bool
    audioMuted() {
        return Audio::isMuted();
    }

    void
    muteAudio() {
        Audio::mute();
    }

    void
    unmuteAudio() {
        Audio::unmute();
    }

    void
    open(const std::string & path) {
        @autoreleasepool {
            NSWorkspace * workspace = [NSWorkspace sharedWorkspace];
            NSURL * url = [NSURL fileURLWithPath:fromStdString(path)];
            [workspace openURL:url];
        }
    }

    void
    openUrl(const std::string & url) {
        @autoreleasepool {
            NSWorkspace * workspace = [NSWorkspace sharedWorkspace];
            NSURL * nsUrl = [NSURL URLWithString:fromStdString(url)];
            if (!nsUrl.scheme)
                nsUrl = [NSURL URLWithString:fromStdString("http://" + url)];
            [workspace openURL:nsUrl];
        }
    }

    std::string
    applicationSupportFolder() {
        @autoreleasepool {
            NSArray * paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
            return toStdString([paths firstObject]);
        }
    }

    void
    revealFile(const std::string & file) {
        @autoreleasepool {
            NSURL * fileURL = [NSURL fileURLWithPath:fromStdString(file)];
            [[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:[NSArray arrayWithObjects:fileURL, nil]];
        }
    }

    void
    postNotificationImp(const std::string & title, const std::string & message, const std::string & icon) {
        @autoreleasepool {
            [(AppDelegate*)NSApp.delegate postMessage: fromStdString(message) withTitle: fromStdString(title) andIcon: fromStdString(icon)];
        }    
    }
}
