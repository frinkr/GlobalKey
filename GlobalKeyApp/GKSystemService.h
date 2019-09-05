#pragma once
#include "GK.h"
#include "GKSystem.h"

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

    template <typename ... T> void
    postNotification(T ... args) {
        auto message = (std::string() + ... + args);
        GKSystem::postNotification("GlobalKey", message);
    }
}
