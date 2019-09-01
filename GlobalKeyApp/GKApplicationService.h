#pragma once

#include "GKHotKey.h"
#include "GKProxyApp.h"

class GKConfig : private GKNoCopy {
private:
    struct TaskEntry {
        GKKeySequence     taskKeySequence;
        std::string       taskCommand;
    };

public:

    const std::string &
    path() const;

    size_t
    taskCount() const;

    const GKKeySequence &
    taskKeySequence(size_t index) const;

    const std::string &
    taskCommand(size_t index) const;

    static const GKConfig&
    instance();

protected:
    GKConfig();

    void
    load();

protected:
    std::string file_;
    std::vector<TaskEntry> entries_;
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

