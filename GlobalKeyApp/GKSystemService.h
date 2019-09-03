#pragma once
#include "GK.h"
#include "GKSystem.h"

namespace GKSystemService {
    void
    adjustVolume(short value);

    void
    muteVolume();

    void
    unmuteVolume();

    template <typename ... T> void
    postNotification(T ... args) {
        auto message = (std::string() + ... + args);
        GKSystem::postNotification("GlobalKey", message);
    }
}
