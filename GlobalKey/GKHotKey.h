#pragma once
#include "GK.h"

class GKHotKey {
public:
    explicit GKHotKey(GKKeySequence keySequence)
        : keySequence_(std::move(keySequence)) {}

    virtual ~GKHotKey() {}

    virtual void
        registerHotKey() = 0;

    virtual void
        unregisterHotKey() = 0;

    virtual void
        invoke() {
        if (handler_)
            handler_();
    }

    void
        setHandler(GKHandler handler) {
        handler_ = handler;
    }

    const GKHandler&
        handler() const {
        return handler_;
    }

protected:
    GKHandler handler_;
    GKKeySequence keySequence_;
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

    virtual GKPtr<GKHotKey>
        createHotKey(const GKKeySequence& keySequence) = 0;

private:
    std::vector<GKPtr<GKHotKey>> hotKeys_;
};
