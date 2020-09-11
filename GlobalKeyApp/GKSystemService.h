#pragma once
#include <sstream>
#include "GK.h"
#include "GKCoreApp.h"

namespace GKSystemService {
    void
    adjustVolume(short value);

    bool
    audioMuted();

    void
    muteAudio();

    void
    unmuteAudio();

    void
    open(const std::string & path);

    void
    openUrl(const std::string & url);

    std::string
    applicationSupportFolder();

    void
    revealFile(const std::string & file);

    void
    postNotificationImp(const std::string& title, const std::string& message, const std::string & icon);

    template <typename ... T> void
    postNotification(T ... args) {
        std::stringstream ss;
        (ss << ... << args);
        postNotificationImp(GKAPP_NAME, ss.str(), "");
    }

#if GK_WIN
    void
    registerAutoRun();

    void
    unregisterAutoRun();

    bool
    isAutoRunRegistered();
#endif
}
