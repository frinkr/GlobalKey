#pragma once
#include "GK.h"


using GKKeySequence = std::string;

class GKHotKey {
public:
    using Handler = std::function<void()>;

public:
    explicit GKHotKey(GKKeySequence keySequence);

    virtual ~GKHotKey();

    virtual void
    registerHotKey();

    virtual void
    unregisterHotKey();

    virtual void
    invoke();

    virtual int
    id() const;

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
