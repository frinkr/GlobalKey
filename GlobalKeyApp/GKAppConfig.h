#pragma once


#include "GKHotKey.h"
#include "GKProxyApp.h"

class GKAppConfig : private GKNoCopy {
private:
    struct CommandEntry {
        GKKeySequence     commandKeySequence;
        std::string       commandText;
    };

public:

    const std::string &
    path() const;

    size_t
    commandCount() const;

    const GKKeySequence &
    commandKeySequence(size_t index) const;

    const std::string &
    commandText(size_t index) const;

    static const GKAppConfig&
    instance();

protected:
    GKAppConfig();

    void
    load();

protected:
    std::string file_;
    std::vector<CommandEntry> entries_;
};
