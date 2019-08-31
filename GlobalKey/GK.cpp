#include "GK.h"
#include "GKProxyApp.h"
#if GK_MAC
#include "GKMac.h"
using GKAppFactoryImp = GKMacAppFactory;
using GKConfigImp = GKMacConfig;
using GKSystemImp = GKMacSystem;
#elif GK_WIN
#include "GKWin.h"
using GKSystemImp = GKWinSystem;
#endif


GKSystem &
GKSystem::instance() {
    static GKSystemImp sys;
    return sys;
}
