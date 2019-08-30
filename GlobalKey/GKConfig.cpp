#include "GKConfig.h"

#if GK_MAC
#  include "GKMacImp.h"
#endif

const GKConfig &
GKConfig::instance() {
    static GKMacConfig config("/Volumes/git/my/GlobalKey/GlobalKey/config.plist");
    return config;
}
