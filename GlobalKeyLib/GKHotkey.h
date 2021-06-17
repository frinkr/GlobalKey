#pragma once
#include <vector>
#include <utility>
#include <optional>

#include "GK.h"

using GKKeySequence = std::string;

class GKHotkey : private GKNoCopy {
public:
    using Ref = void *;
    using Handler = std::function<void()>;

public:
    explicit GKHotkey(GKKeySequence keySequence, bool autoRepeat = false);

    virtual ~GKHotkey();

    const GKKeySequence &
    keySequence() const;
    
    virtual GKErr
    registerHotkey();

    virtual GKErr
    unregisterHotkey();

    virtual bool
    isRegistered() const;
    
    virtual void
    invoke();

    virtual Ref
    ref() const;

    void
    setHandler(Handler handler);

    const Handler &
    handler() const;
    
protected:
    Handler handler_ {};
    GKKeySequence keySequence_ {};
    bool autoRepeat_ {};
    bool registered_ {};
    class Imp;
    friend class Imp;
    std::unique_ptr<Imp> imp_;
};

enum GKHotkeyModifier {
    kSHIFT  =  0x01000000,
    kCTRL   =  kSHIFT << 1,
    kALT    =  kSHIFT << 2,
    kMETA   =  kSHIFT << 3,
};

// Key code names
constexpr const char * kF1        = "F1";
constexpr const char * kF2        = "F2";
constexpr const char * kF3        = "F3";
constexpr const char * kF4        = "F4";
constexpr const char * kF5        = "F5";
constexpr const char * kF6        = "F6";
constexpr const char * kF7        = "F7";
constexpr const char * kF8        = "F8";
constexpr const char * kF9        = "F9";
constexpr const char * kF10       = "F10";
constexpr const char * kF11       = "F11";
constexpr const char * kF12       = "F12";
constexpr const char * kF13       = "F13";
constexpr const char * kF14       = "F14";
constexpr const char * kF15       = "F15";
constexpr const char * kF16       = "F16";
constexpr const char * kF17       = "F17";
constexpr const char * kF18       = "F18";
constexpr const char * kF19       = "F19";
constexpr const char * kF20       = "F20";
constexpr const char * kF21       = "F21";
constexpr const char * kF22       = "F22";
constexpr const char * kF23       = "F23";
constexpr const char * kF24       = "F24";
constexpr const char * kSpace     = "SPACE";
constexpr const char * kLeft      = "LEFT";
constexpr const char * kRight     = "RIGHT";
constexpr const char * kUp        = "UP";
constexpr const char * kDown      = "DOWN";
constexpr const char * kPageUp    = "PAGEUP";
constexpr const char * kPageDown  = "PAGEDOWN";

std::optional<std::pair<GKHotkeyModifier, std::string> >
GKSplitKeySequence(const GKKeySequence& commandKeySequence);
