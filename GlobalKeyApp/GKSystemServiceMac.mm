#import <AppKit/AppKit.h>
#import <AudioToolbox/AudioServices.h>
#import <Carbon/Carbon.h>
#import <CoreServices/CoreServices.h>
#import <IOKit/pwr_mgt/IOPMLib.h>
// #import <IOKit/pwr_mgt/IOPMLibPrivate.h>

#include <cmath>
#include <algorithm>
#include <dlfcn.h>

#include "GKArgs.h"
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

    OSStatus SendAppleEventToSystemProcess(AEEventID EventToSend) {
        AEAddressDesc targetDesc;
        static const ProcessSerialNumber kPSNOfSystemProcess = { 0, kSystemProcess };
        AppleEvent eventReply = {typeNull, NULL};
        AppleEvent appleEventToSend = {typeNull, NULL};

        OSStatus error = noErr;

        error = AECreateDesc(typeProcessSerialNumber, &kPSNOfSystemProcess, 
                             sizeof(kPSNOfSystemProcess), &targetDesc);

        if (error != noErr)
            return(error);


        error = AECreateAppleEvent(kCoreEventClass, EventToSend, &targetDesc, 
                                   kAutoGenerateReturnID, kAnyTransactionID, &appleEventToSend);

        AEDisposeDesc(&targetDesc);
        if (error != noErr)

            return(error);


        error = AESend(&appleEventToSend, &eventReply, kAENoReply, 
                       kAENormalPriority, kAEDefaultTimeout, NULL, NULL);

        AEDisposeDesc(&appleEventToSend);
        if (error != noErr)

            return(error);


        AEDisposeDesc(&eventReply);
        return(error);
    }
    
}

namespace Audio {
    AudioDeviceID defaultOutputDeviceID() {
        // get output device device
        AudioObjectPropertyAddress address;
        address.mSelector = kAudioHardwarePropertyDefaultOutputDevice;
        address.mScope = kAudioObjectPropertyScopeGlobal;
        address.mElement = kAudioObjectPropertyElementMain;
	
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
        address.mElement = kAudioObjectPropertyElementMain;
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
        address.mElement = kAudioObjectPropertyElementMain;
        if (!AudioObjectHasProperty(outputDeviceID, &address))
            return kAudioHardwareUnknownPropertyError;

        UInt32 propertySize = sizeof(value);
        return AudioObjectGetPropertyData(outputDeviceID, &address, 0, NULL, &propertySize, &value);
    }

    float getVolume() {
        Float32 outputVolume;
        OSStatus status = getAudioProperty(kAudioHardwareServiceDeviceProperty_VirtualMainVolume, outputVolume);
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
        setAudioProperty<Float32>(kAudioHardwareServiceDeviceProperty_VirtualMainVolume, newVolume);
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
        
        postNotification("Volume", "Volume " + std::to_string(vol), "vol");
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
            GKArgs gkArgs(path);
            NSMutableArray<NSString*> * args = [[NSMutableArray alloc] init];
            while (!gkArgs.done())
                [args addObject:fromStdString(gkArgs.take_head())];
            
            NSTask * task = [[NSTask alloc] init];
            [task setLaunchPath: @"/usr/bin/open"];
            [task setArguments:args];
            [task launch];
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

    void
    lockScreen() {
        using SACLockScreenImmediateFunc = int (*) ();
        static SACLockScreenImmediateFunc SACLockScreenImmediate = [] {
            if (auto lib = dlopen("/System/Library/PrivateFrameworks/login.framework/Versions/Current/login", RTLD_GLOBAL | RTLD_NOW)) {
                auto func = (SACLockScreenImmediateFunc)(dlsym(lib, "SACLockScreenImmediate"));
                dlclose(lib);
                return func;
            }
            return SACLockScreenImmediateFunc(nullptr);
        }();

        if (SACLockScreenImmediate)
            SACLockScreenImmediate();
    }

    void
    computerSleep() {
        SendAppleEventToSystemProcess(kAESleep);
    }

    IOPMAssertionID sKeepAwakeAssertionID = 0;
    
    void
    keepComputerAwake()
    {
        if (isKeepingComputerAwake())
            return;
        
        IOReturn result = IOPMAssertionCreateWithDescription(
                                                             kIOPMAssertPreventUserIdleSystemSleep,
            CFSTR("Global Key"),
            CFSTR("Global Key asserting forever"),
            CFSTR("THE GLOBAL KEY IS PREVENTING SLEEP."),
            CFSTR("/System/Library/CoreServices/powerd.bundle"),
            (CFTimeInterval) 0,
            kIOPMAssertionTimeoutActionRelease,
            &sKeepAwakeAssertionID
            );
        if (kIOReturnSuccess != result)
            sKeepAwakeAssertionID = 0;
    }

    bool
    isKeepingComputerAwake()
    {
        return sKeepAwakeAssertionID != 0;
    }

    void
    stopKeepingComputerAwake()
    {
        if (sKeepAwakeAssertionID) {
            if (kIOReturnSuccess == IOPMAssertionRelease(sKeepAwakeAssertionID))
                sKeepAwakeAssertionID = 0;
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
    postNotification(const std::string & title, const std::string & message, const std::string & icon) {
        @autoreleasepool {
            [(AppDelegate*)NSApp.delegate postMessage: fromStdString(message) withTitle: fromStdString(title) andIcon: fromStdString(icon)];
        }    
    }
}
