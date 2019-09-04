#import <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>
#include <Carbon/Carbon.h>

#include <array>
#include <string_view>
#include <stdexcept>
#include <map>
#include "GKMac.h"

namespace {
    NSString *
    fromStdString(const std::string & str) {
        return [NSString stringWithCString:str.c_str()
                                   encoding:[NSString defaultCStringEncoding]];
    }

    std::string
    toStdString(NSString * str) {
        return std::string([str UTF8String]);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//                             GKAppProxy::Imp:

struct GKAppProxy::Imp::MacImp {
public:
    MacImp(GKAppProxy * parent)
    : parent_(parent) {}
    
    NSRunningApplication *
    runningApp() const {
        NSString * path = fromStdString(parent_->descriptor());
        
        NSString * bundleIdentifier = nil;
        NSBundle * bundle = [NSBundle bundleWithPath:path];
        if (bundle)
            bundleIdentifier = [bundle bundleIdentifier];
        else
            bundleIdentifier = path;
        
        // Search bundle
        if (bundleIdentifier) {
            NSArray<NSRunningApplication *> * running = [NSRunningApplication runningApplicationsWithBundleIdentifier:bundleIdentifier];
            if ([running count])
                return [running objectAtIndex:0];
        }
        
        
        // Not a bundle, search by executable path
        NSWorkspace * workspace = [NSWorkspace sharedWorkspace];
        NSArray<NSRunningApplication *> * runningApps = [workspace runningApplications];
        for (NSUInteger i = 0; i < [runningApps count]; ++ i) {
            NSRunningApplication * app = [runningApps objectAtIndex:i];
            if ([app.executableURL.path isEqualToString:path])
                return app;
        }
        return nil;
    }
    
    static NSRunningApplication *
    frontmostApp() {
        NSWorkspace * workspace = [NSWorkspace sharedWorkspace];
        return [workspace frontmostApplication];
    }
private:
    GKAppProxy * parent_ {};
};

GKAppProxy::Imp::Imp(GKAppProxy * parent)
    : parent_(parent)
    , imp_(std::make_unique<GKAppProxy::Imp::MacImp>(parent))
    {}

GKAppProxy::Imp::~Imp() = default;


GKErr
GKAppProxy::Imp::bringFront() {
    @autoreleasepool {
        NSRunningApplication * app = imp_->runningApp();
        if (!app)
            return GKErr::appCantFound;
        
        // NSApplicationActivateAllWindows not work for Emacs.app on 10.15 Beta (19A546d)
        if ([app activateWithOptions:NSApplicationActivateIgnoringOtherApps])
            return GKErr::noErr;
        else
            return GKErr::appCantActivate;
    }
}

GKErr
GKAppProxy::Imp::show() {
    @autoreleasepool {
        NSRunningApplication * app = imp_->runningApp();
        if ([app unhide])
            return GKErr::noErr;
        else
            return GKErr::appCantFound;
    }
}

GKErr
GKAppProxy::Imp::hide() {
    @autoreleasepool {
        NSRunningApplication * app = imp_->runningApp();
        if ([app hide])
            return GKErr::noErr;
        else
            return GKErr::appCantFound;
    }
}

bool
GKAppProxy::Imp::visible() const  {
    @autoreleasepool {
        NSRunningApplication * app = imp_->runningApp();
        if (app)
            return not [app isHidden];
        else
            return false;
    }
}

bool
GKAppProxy::Imp::atFrontmost() const {
    @autoreleasepool {
        NSRunningApplication * app = imp_->runningApp();
        NSRunningApplication * front = MacImp::frontmostApp();
        return [[app bundleURL] isEqualTo:[front bundleURL]];
    }
}

bool
GKAppProxy::Imp::running() const  {
    @autoreleasepool {
        return nil != imp_->runningApp();
    }
}

GKErr
GKAppProxy::Imp::launch() {
    @autoreleasepool {
        NSWorkspace * workspace = [NSWorkspace sharedWorkspace];
        
        NSString * path = fromStdString(parent_->descriptor());
        NSURL * url = [workspace URLForApplicationWithBundleIdentifier:path];
        if (!url)
            url = [NSURL fileURLWithPath:path];
        
        BOOL ok = [workspace openURL:url];
        return ok? GKErr::noErr: GKErr::appCantLaunch;
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//                             GKHotKey::Imp:

namespace {
    
    
    uint64_t
    encodeEventHotKeyID(const EventHotKeyID & keyId) {
        uint64_t v = keyId.signature;
        v = v << 32;
        v += keyId.id;
        return v;
    }
    
    class GKHotKeyManagerMac {
    public:
        static GKHotKeyManagerMac &
        instance() {
            static GKHotKeyManagerMac instance;
            return instance;
        }
        
        
        static OSStatus hotkeyEventHandler(EventHandlerCallRef nextHandler, EventRef event, void* data)
        {
            if (GetEventClass(event) == kEventClassKeyboard &&
                GetEventKind(event) == kEventHotKeyPressed) {
                EventHotKeyID hkeyID;
                GetEventParameter(event,
                                  kEventParamDirectObject,
                                  typeEventHotKeyID,
                                  NULL,
                                  sizeof(EventHotKeyID),
                                  NULL,
                                  &hkeyID);
                
                instance().invokeHotKey(hkeyID);
            }
            
            return noErr;
        }
        
        bool installHotkeyEventHandler() {
            if (hotkeyEventHandlerInstalled_)
                return true;
            
            hotkeyEventHandlerInstalled_ = true;
            EventTypeSpec eventSpec;
            eventSpec.eventClass = kEventClassKeyboard;
            eventSpec.eventKind = kEventHotKeyPressed;
            if (InstallApplicationEventHandler(&hotkeyEventHandler, 1, &eventSpec, NULL, NULL))
                return false;
            else
                return true;
        }
        
        void
        addHotkey(EventHotKeyID id, GKHotKey * hotkey) {
            registeredHotKeys_[encodeEventHotKeyID(id)] = hotkey;
        }
        
        void
        removeHotKey(GKHotKey * hotkey) {
            for (auto itr = registeredHotKeys_.begin(); itr != registeredHotKeys_.end(); ++ itr) {
                if (itr->second == hotkey) {
                    registeredHotKeys_.erase(itr);
                    break;
                }
            }
        }
        
        void
        invokeHotKey(EventHotKeyID keyId) {
            uint64_t enc = encodeEventHotKeyID(keyId);
            if (auto itr = registeredHotKeys_.find(enc); itr != registeredHotKeys_.end()) {
                itr->second->invoke();
            }
            
        }
        
    private:
        bool hotkeyEventHandlerInstalled_ {false};
        
        std::map<uint64_t, GKHotKey *> registeredHotKeys_;
    };
    
    uint32_t kVK_Invalid = -1;

    std::pair<uint32_t, uint32_t>
    parseKeySequence(const GKKeySequence& keySequence) {
        auto p = GKSplitKeySequence(keySequence);
        if (!p)
            return {0, kVK_Invalid};
        
        auto [mod, key] = *p;

        uint32_t macMod = 0;
        if (mod & kSHIFT) macMod |= shiftKey;
        if (mod & kALT) macMod |= optionKey;
        if (mod & kCTRL) macMod |= controlKey;
        if (mod & kMETA) macMod |= cmdKey;

        static const std::map<std::string, uint32_t> map {
            {"F1", kVK_F1},
            {"F2", kVK_F2},
            {"F3", kVK_F3},
            {"F4", kVK_F4},
            {"F5", kVK_F5},
            {"F6", kVK_F6},
            {"F7", kVK_F7},
            {"F8", kVK_F8},
            {"F9", kVK_F9},
            {"F10", kVK_F10},
            {"F11", kVK_F11},
            {"F12", kVK_F12},
            {"F13", kVK_F13},
            {"F14", kVK_F14},
            {"F15", kVK_F15},
            {"F16", kVK_F16},
            {"F17", kVK_F17},
            {"F18", kVK_F18},
            {"F19", kVK_F19},
            {"F20", kVK_F20},
            {"SPACE", kVK_Space},
            {"LEFT", kVK_LeftArrow},
            {"RIGHT", kVK_RightArrow},
            {"DOWN", kVK_DownArrow},
            {"UP", kVK_UpArrow},
            {"PAGEUP", kVK_PageUp},
            {"PAGEDOWN", kVK_PageDown},
        };

        if (auto itr = map.find(key); itr != map.end())
            return { macMod, itr->second };

        if (key.size() == 1) {

            UTF16Char ch = key.front();

            CFDataRef currentLayoutData;
            TISInputSourceRef currentKeyboard = TISCopyCurrentKeyboardInputSource();

            if (currentKeyboard == NULL)
                return { 0, kVK_Invalid };

            currentLayoutData = (CFDataRef)TISGetInputSourceProperty(currentKeyboard, kTISPropertyUnicodeKeyLayoutData);
            CFRelease(currentKeyboard);
            if (currentLayoutData == NULL)
                return { 0, kVK_Invalid };

            UCKeyboardLayout* header = (UCKeyboardLayout*)CFDataGetBytePtr(currentLayoutData);
            UCKeyboardTypeHeader* table = header->keyboardTypeList;

            uint8_t *data = (uint8_t*)header;
            for (int32_t i=0; i < header->keyboardTypeCount; i++) {
                UCKeyStateRecordsIndex* stateRec = 0;
                if (table[i].keyStateRecordsIndexOffset != 0) {
                    stateRec = reinterpret_cast<UCKeyStateRecordsIndex*>(data + table[i].keyStateRecordsIndexOffset);
                    if (stateRec->keyStateRecordsIndexFormat != kUCKeyStateRecordsIndexFormat) stateRec = 0;
                }

                UCKeyToCharTableIndex* charTable = reinterpret_cast<UCKeyToCharTableIndex*>(data + table[i].keyToCharTableIndexOffset);
                if (charTable->keyToCharTableIndexFormat != kUCKeyToCharTableIndexFormat) continue;

                for (int32_t j=0; j < charTable->keyToCharTableCount; j++) {
                    UCKeyOutput* keyToChar = reinterpret_cast<UCKeyOutput*>(data + charTable->keyToCharTableOffsets[j]);
                    for (uint32_t vk=0; vk < charTable->keyToCharTableSize; vk++) {
                        if (keyToChar[vk] & kUCKeyOutputTestForIndexMask) {
                            long idx = keyToChar[vk] & kUCKeyOutputGetIndexMask;
                            if (stateRec && idx < stateRec->keyStateRecordCount) {
                                UCKeyStateRecord* rec = reinterpret_cast<UCKeyStateRecord*>(data + stateRec->keyStateRecordOffsets[idx]);
                                if (rec->stateZeroCharData == ch)
                                    return {macMod, vk};
                            }
                        }
                        else if (!(keyToChar[vk] & kUCKeyOutputSequenceIndexMask) && keyToChar[vk] < 0xFFFE) {
                            if (keyToChar[vk] == ch) 
                                return {macMod, vk};
                        }
                    }
                }
            }
        }
        return { 0, kVK_Invalid };
    }
}

GKHotKey::Imp::Imp(GKHotKey * parent)
: parent_(parent) {
    std::tie(mod_, key_) = parseKeySequence(parent_->keySequence_);
}

GKErr
GKHotKey::Imp::registerHotKey() {
    if (key_ == kVK_Invalid)
        return GKErr::hotKeySequenceNotValid;
        
    if (!GKHotKeyManagerMac::instance().installHotkeyEventHandler())
        return GKErr::hotKeyCantRegister;
    
    EventHotKeyID hkeyID;
    hkeyID.signature = key_;
    hkeyID.id = mod_;
    
    EventHotKeyRef eventRef = 0;
    OSStatus status = RegisterEventHotKey(key_,
                                          mod_,
                                          hkeyID,
                                          GetApplicationEventTarget(),
                                          0,
                                          &eventRef);
    
    if (status == eventHotKeyExistsErr)
        return GKErr::hotKeyExists;
    if (status != 0)
        return GKErr::hotKeyCantRegister;
    
    ref_ = eventRef;
    GKHotKeyManagerMac::instance().addHotkey(hkeyID, parent_);
    return GKErr::noErr;
}

GKErr
GKHotKey::Imp::unregisterHotKey() {
    if (key_ == kVK_Invalid)
        return GKErr::noErr;
    
    OSStatus status = UnregisterEventHotKey(EventHotKeyRef(ref_));
    if (status != 0)
        return GKErr::hotKeyCantUnregisteer;
    
    GKHotKeyManagerMac::instance().removeHotKey(parent_);
    return GKErr::noErr;
}

GKHotKey::Ref
GKHotKey::Imp::ref() const {
    return ref_;
}


//////////////////////////////////////////////////////////////////////////////////////////
//                            GKSystemImp

void
GKSystemImp::postNotification(const std::string & title, const std::string & message) {
    @autoreleasepool {
        NSUserNotification * notification = [[NSUserNotification alloc] init];
        notification.title = fromStdString(title);
        notification.informativeText = fromStdString(message);
        notification.soundName = NSUserNotificationDefaultSoundName;
        [[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification: notification];
    }
}

std::string
GKSystemImp::applicationSupportFolder() {
    @autoreleasepool {
        NSArray * paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
        return toStdString([paths firstObject]);
    }
}

void
GKSystemImp::revealFile(const std::string & file) {
    @autoreleasepool {
        NSURL * fileURL = [NSURL fileURLWithPath:fromStdString(file)];
        [[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:[NSArray arrayWithObjects:fileURL, nil]];
    }
}
