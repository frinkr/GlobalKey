#include "GKConfig.h"
#if GK_MAC
#include "GKMac.h"
using GKConfigImp = GKMacConfig;
#elif GK_WIN
#include "GKWin.h"
using GKConfigImp = GKWinConfig;
#endif

const GKConfig&
GKConfig::instance() {
    static std::unique_ptr<GKConfigImp> config;
    if (!config) {
        config = std::make_unique<GKConfigImp>();
        config->load();
    }
    return *config;
}

std::string
GKConfig::path() const {
    return file_;
}

size_t
GKConfig::appCount() const {
    return entries_.size();
}

const std::string&
GKConfig::appKeySequence(size_t index) const {
    return entries_[index].keySequence;
}


const GKAppDescriptor&
GKConfig::appDescriptor(size_t index) const {
    return entries_[index].appDescriptor;
}
