#pragma once

#include "GKApp.h"

class GKConfig {
public:
    virtual ~GKConfig() {}

    virtual size_t
    appCount() const = 0;

    virtual std::string
    appKeySequence(size_t index) const = 0;
    
    virtual GKPtr<const GKAppId>
    appId(size_t index) const = 0;

    static const GKConfig &
    instance();
};
