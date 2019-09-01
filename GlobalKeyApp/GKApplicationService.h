#pragma once

#include "GKHotKey.h"
#include "GKProxyApp.h"

class GKConfig : private GKNoCopy {
private:
    struct Entry {
        GKKeySequence     keySequence;
        std::string       taskDescriptor;
    };

public:

    const std::string &
    path() const;

    size_t
    appCount() const;

    const GKKeySequence &
    keySequence(size_t index) const;

    const std::string &
    taskDescriptor(size_t index) const;

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

