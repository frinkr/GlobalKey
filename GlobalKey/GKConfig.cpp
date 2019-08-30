#include "GKConfig.h"

#if GK_MAC
#  include "GKMacImp.h"
#endif

const GKConfig &
GKConfig::instance() {
    static GKMacConfig config;
    return config;
}
