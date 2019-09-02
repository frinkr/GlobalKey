#pragma once

#include "GKHotKey.h"

class GKAppHotKeyManager : private GKNoCopy {
public:
    static GKAppHotKeyManager&
    instance();

    virtual ~GKAppHotKeyManager();

    virtual void
    loadHotKeys();

    virtual void
    registerHotKeys();

    virtual void
    unregisterHotKeys();

    const std::vector<GKPtr<GKHotKey>>&
    hotKeys() const;

private:
    std::vector<GKPtr<GKHotKey>> hotKeys_;
};

