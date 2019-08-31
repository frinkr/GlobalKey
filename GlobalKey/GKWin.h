#pragma once

#include <Windows.h>
#include "GK.h"


class GKWinAppFactory : public GKAppFactory {
public:
    GKPtr<GKApp>
    getOrCreateApp(const GKAppDescriptor & appDescriptor) override;
};

class GKWinHotKey : public GKHotKey {
public:
    GKWinHotKey(HWND hwnd, const GKKeySequence& keySequence);

    void
    registerHotKey() override;

    void
    unregisterHotKey() override;

    int 
    id() const override;
private:
    HWND hwnd_{};
    int modifiers_{};
    int virtualKey_{};
};

class GKWinHotKeyManager: public GKHotKeyManager {
public:
    void
    setHWND(HWND hwnd);

    HWND
    hwnd() const;

    GKPtr<GKHotKey>
    createHotKey(const std::string & keySequence) override;

private:
    HWND  hwnd_{};
};

class GKWinConfig : public GKConfig {
protected:
    friend GKConfig;

    void
    load() override;
};


class GKWinSystem : public GKSystem {
public:
    void
    postNotification(const std::string & title, const std::string & message) override;
};
