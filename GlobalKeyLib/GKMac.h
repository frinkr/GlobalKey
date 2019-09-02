#pragma once
#include <cstdint>
#include <vector>

#include "GK.h"
#include "GKProxyApp.h"
#include "GKHotKey.h"

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


class GKHotKey::Imp {
public:
    explicit Imp(GKHotKey * parent);
    
    void
    registerHotKey();
    
    void
    unregisterHotKey();
    
    GKHotKey::Ref
    ref() const;
    
private:
    GKHotKey * parent_{};
    
    GKHotKey::Ref ref_ {};
    uint32_t key_ {};
    uint32_t mod_ {};
};

class GKSystemImp {
public:
    static void
    postNotification(const std::string & title, const std::string & message);
};
