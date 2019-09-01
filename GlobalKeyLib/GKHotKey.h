#pragma once
#include <vector>
#include <utility>

#include "GK.h"

using GKKeySequence = std::string;

class GKHotKey : private GKNoCopy {
public:
    using Ref = void *;
    using Handler = std::function<void()>;

public:
    explicit GKHotKey(GKKeySequence taskKeySequence);

    virtual ~GKHotKey();

    virtual void
    registerHotKey();

    virtual void
    unregisterHotKey();

    virtual void
    invoke();

    virtual Ref
    ref() const;

    void
    setHandler(Handler handler);

    const Handler &
    handler() const;
    
protected:
    Handler handler_;
    GKKeySequence keySequence_;
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

std::pair<GKHotKeyModifier, std::string>
GKSplitKeySequence(const GKKeySequence& taskKeySequence);
