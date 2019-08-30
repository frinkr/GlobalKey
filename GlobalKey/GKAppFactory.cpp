#include "GKAppFactory.h"

#if GK_MAC
#  include "GKMacImp.h"
#endif


GKAppFactory & GKAppFactory::instance() {
    static GKMacAppFactory appSwitch;
    return appSwitch;
}
