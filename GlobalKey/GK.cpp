#include "GK.h"

#if GK_MAC
#  include "GKMac.h"
#endif

#if GK_MAC
const GKConfig &
GKConfig::instance() {
    static GKMacConfig config;
    return config;
}


GKAppFactory & GKAppFactory::instance() {
    static GKMacAppFactory appSwitch;
    return appSwitch;
}


GKSystem &
GKSystem::instance() {
    static GKMacSystem sys;
    return sys;
}
#endif