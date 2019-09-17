#pragma once

#include "GKProxyApp.h"

class GKProxyApp::Imp{
public:
    explicit Imp(GKProxyApp * parent);

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
    GKProxyApp * parent_ {};
    struct MacImp;
    std::unique_ptr<MacImp> imp_;
};
