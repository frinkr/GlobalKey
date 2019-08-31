#pragma once

#include "GK.h"
#include "GKProxyApp.h"
#include "GKHotKey.h"

class GKConfig {
private:
    struct Entry {
        GKKeySequence     keySequence;
        GKAppDescriptor   appDescriptor;
    };

public:
    virtual ~GKConfig() {}

    virtual std::string
    path() const;

    virtual size_t
    appCount() const;

    virtual const GKKeySequence&
    appKeySequence(size_t index) const;

    virtual const GKAppDescriptor&
    appDescriptor(size_t index) const;

    static const GKConfig&
    instance();

protected:
    virtual void
    load() = 0;

protected:
    std::string file_;
    std::vector<Entry> entries_;
};
