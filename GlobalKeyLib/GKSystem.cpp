#include "GKSystem.h"

#if GK_MAC
#  include "GKMac.h"
#elif GK_WIN
#  include "GKWin.h"
#endif

void
GKSystem::postNotification(const std::string& title, const std::string& message) {
    return GKSystemImp::postNotification(title, message);
}

std::string
GKSystem::applicationSupportFolder() {
    return GKSystemImp::applicationSupportFolder();
}

void
GKSystem::revealFile(const std::string & file) {
    return GKSystemImp::revealFile(file);
}
