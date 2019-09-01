#include "GKSystem.h"

#if GK_MAC
#include "GKMac.h"
#elif GK_WIN
#include "GKWin.h"
#endif


void
GKSystem::postNotification(const std::string& title, const std::string& message) {
    GKSystemImp::postNotification(title, message);
}