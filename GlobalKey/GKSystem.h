#pragma once

#include "GK.h"

class GKSystem : private GKNoCopy {
public:
    static void
    postNotification(const std::string& title, const std::string& message);
};
