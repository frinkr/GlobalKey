#pragma once
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
    postNotificationImp(const std::string& title, const std::string& message);

    template <typename ... T> void
    postNotification(T ... args) {
        auto message = (std::string() + ... + args);
        postNotificationImp("GlobalKey", message);
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
