#import <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>
#include <Carbon/Carbon.h>
#include <map>

#include "GKHotkeyMac.h"

namespace {
    uint64_t
    encodeEventHotkeyID(const EventHotKeyID & keyId) {
        uint64_t v = keyId.signature;
        v = v << 32;
        v += keyId.id;
        return v;
    }
    
    class GKHotkeyManagerMac {
    public:
        static GKHotkeyManagerMac &
        instance() {
            static GKHotkeyManagerMac instance;
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
                
                instance().invokeHotkey(hkeyID);
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
        addHotkey(EventHotKeyID id, GKHotkey * hotkey) {
            registeredHotkeys_[encodeEventHotkeyID(id)] = hotkey;
        }
        
        void
        removeHotkey(GKHotkey * hotkey) {
            for (auto itr = registeredHotkeys_.begin(); itr != registeredHotkeys_.end(); ++ itr) {
                if (itr->second == hotkey) {
                    registeredHotkeys_.erase(itr);
                    break;
                }
            }
        }
        
        void
        invokeHotkey(EventHotKeyID keyId) {
            uint64_t enc = encodeEventHotkeyID(keyId);
            if (auto itr = registeredHotkeys_.find(enc); itr != registeredHotkeys_.end()) {
                itr->second->invoke();
            }
            
        }
        
    private:
        bool hotkeyEventHandlerInstalled_ {false};
        
        std::map<uint64_t, GKHotkey *> registeredHotkeys_;
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
            {kF1, kVK_F1},
            {kF2, kVK_F2},
            {kF3, kVK_F3},
            {kF4, kVK_F4},
            {kF5, kVK_F5},
            {kF6, kVK_F6},
            {kF7, kVK_F7},
            {kF8, kVK_F8},
            {kF9, kVK_F9},
            {kF10, kVK_F10},
            {kF11, kVK_F11},
            {kF12, kVK_F12},
            {kF13, kVK_F13},
            {kF14, kVK_F14},
            {kF15, kVK_F15},
            {kF16, kVK_F16},
            {kF17, kVK_F17},
            {kF18, kVK_F18},
            {kF19, kVK_F19},
            {kF20, kVK_F20},
            {kSpace, kVK_Space},
            {kLeft, kVK_LeftArrow},
            {kRight, kVK_RightArrow},
            {kDown, kVK_DownArrow},
            {kUp, kVK_UpArrow},
            {kPageUp, kVK_PageUp},
            {kPageDown, kVK_PageDown},
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

GKHotkey::Imp::Imp(GKHotkey * parent)
: parent_(parent) {
    std::tie(mod_, key_) = parseKeySequence(parent_->keySequence_);
}

GKErr
GKHotkey::Imp::registerHotkey() {
    if (key_ == kVK_Invalid)
        return GKErr::hotkeySequenceNotValid;
        
    if (!GKHotkeyManagerMac::instance().installHotkeyEventHandler())
        return GKErr::hotkeyCantRegister;
    
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
        return GKErr::hotkeyExists;
    if (status != 0)
        return GKErr::hotkeyCantRegister;
    
    ref_ = eventRef;
    GKHotkeyManagerMac::instance().addHotkey(hkeyID, parent_);
    return GKErr::noErr;
}

GKErr
GKHotkey::Imp::unregisterHotkey() {
    if (key_ == kVK_Invalid)
        return GKErr::noErr;
    
    OSStatus status = UnregisterEventHotKey(EventHotKeyRef(ref_));
    if (status != 0)
        return GKErr::hotkeyCantUnregisteer;
    
    GKHotkeyManagerMac::instance().removeHotkey(parent_);
    return GKErr::noErr;
}

GKHotkey::Ref
GKHotkey::Imp::ref() const {
    return ref_;
}
