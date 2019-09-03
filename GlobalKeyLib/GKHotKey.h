#pragma once
#include <vector>
#include <utility>
#include <optional>

#include "GK.h"

using GKKeySequence = std::string;

class GKHotKey : private GKNoCopy {
public:
    using Ref = void *;
    using Handler = std::function<void()>;

public:
    explicit GKHotKey(GKKeySequence keySequence);

    virtual ~GKHotKey();

    const GKKeySequence &
    keySequence() const;
    
    virtual GKErr
    registerHotKey();

    virtual GKErr
    unregisterHotKey();

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
    bool registered_ {};
    class Imp;
    friend class Imp;
    std::unique_ptr<Imp> imp_;
};

enum GKHotKeyModifier {
    kSHIFT  =  0x01000000,
    kCTRL   =  kSHIFT << 1,
    kALT    =  kSHIFT << 2,
    kMETA   =  kSHIFT << 3,
};

std::optional<std::pair<GKHotKeyModifier, std::string> >
GKSplitKeySequence(const GKKeySequence& commandKeySequence);
