#pragma once

#include "GKApp.h"

class GKAppFactory {
public:
    static GKAppFactory &
    instance();

    virtual ~GKAppFactory() {};
    
    virtual GKPtr<GKApp>
    getOrCreateApp(GKPtr<const GKAppId> appId) = 0;
};
