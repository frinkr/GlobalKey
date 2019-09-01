#pragma once

#include "GKHotKey.h"
#include "GKProxyApp.h"

class GKConfig : private GKNoCopy {
private:
    struct Entry {
        GKKeySequence     keySequence;
        GKAppDescriptor   appDescriptor;
    };

public:

    const std::string &
    path() const;

    size_t
    appCount() const;

    const GKKeySequence&
    appKeySequence(size_t index) const;

    const GKAppDescriptor&
    appDescriptor(size_t index) const;

    static const GKConfig&
    instance();

protected:
    GKConfig();

    void
    load();

protected:
    std::string file_;
    std::vector<Entry> entries_;
};


class GKHotKeyManager {
public:
    static GKHotKeyManager&
    instance();

    virtual ~GKHotKeyManager();

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

