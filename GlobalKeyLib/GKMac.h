#pragma once
#include <cstdint>
#include <vector>

#include "GK.h"
#include "GKProxyApp.h"
#include "GKHotkey.h"

class GKAppProxy::Imp{
public:
    explicit Imp(GKAppProxy * parent);

    ~Imp();
    
    GKErr
    bringFront();

    GKErr
    show();

    GKErr
    hide();

    bool
    visible() const;

    bool
    atFrontmost() const;

    bool
    running() const;

    GKErr
    launch();

private:
    GKAppProxy * parent_ {};
    struct MacImp;
    std::unique_ptr<MacImp> imp_;
};


class GKHotkey::Imp {
public:
    explicit Imp(GKHotkey * parent);
    
    GKErr
    registerHotkey();
    
    GKErr
    unregisterHotkey();
    
    GKHotkey::Ref
    ref() const;
    
private:
    GKHotkey * parent_{};
    
    GKHotkey::Ref ref_ {};
    uint32_t key_ {};  // -1 means invalid
    uint32_t mod_ {};
};

class GKSystemImp {
public:
    static void
    postNotification(const std::string & title, const std::string & message);

    static std::string
    applicationSupportFolder();

    static void
    revealFile(const std::string & file);
};
