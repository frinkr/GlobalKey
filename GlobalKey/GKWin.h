#pragma once

#include <Windows.h>
#include "GK.h"

class GKWinApp : public GKApp {
public:
    explicit GKWinApp(const GKAppDescriptor & descriptor);

    ~GKWinApp();
    
    GKErr
    bringFront() override;

    GKErr
    show() override;

    GKErr
    hide() override;

    bool
    visible() const override;

    bool
    atFrontmost() const override;

    bool
    running() const override;

    GKErr
    launch() override;

private:
    struct Imp;
    std::unique_ptr<Imp> imp_;
};


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
    id() const;
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
