#include "GK.h"

#if GK_MAC
#include "GKMac.h"
using GKAppFactoryImp = GKMacAppFactory;
using GKConfigImp = GKMacConfig;
using GKSystemImp = GKMacSystem;
#elif GK_WIN
#include "GKWin.h"
using GKAppFactoryImp = GKWinAppFactory;
using GKConfigImp = GKWinConfig;
using GKSystemImp = GKWinSystem;
using GKHotKeyManagerImp = GKWinHotKeyManager;
#endif

namespace {
    class GKToggleAppTask {
    public:
        explicit GKToggleAppTask(size_t appIndex)
            : appIndex_(appIndex) {}

        void
        operator()() {
            auto appDescriptor = GKConfig::instance().appDescriptor(appIndex_);
            auto app = GKAppFactory::instance().getOrCreateApp(appDescriptor);
            if (!app) {
                GKSystem::instance().postNotification("GlobalKey", "Failed to find application " + appDescriptor);
                return;
            }

            if (!app->running()) 
                if (GKErr::noErr != app->launch()) {
                    GKSystem::instance().postNotification("GlobalKey", "Failed to launch application " + appDescriptor);
                    return;
                }
            if (app->atFrontmost())
                app->hide();
            else
                app->bringFront();    
        }

    private:
        size_t appIndex_;
    };
}

GKApp::GKApp(const GKAppDescriptor & descriptor)
    : descriptor_(descriptor) {
}

const GKAppDescriptor &
GKApp::descriptor() const {
    return descriptor_;
}


GKAppFactory & GKAppFactory::instance() {
    static GKAppFactoryImp appSwitch;
    return appSwitch;
}

const GKConfig &
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

const std::string &
GKConfig::appKeySequence(size_t index) const {
    return entries_[index].keySequence;
}


const GKAppDescriptor &
GKConfig::appDescriptor(size_t index) const {
    return entries_[index].appDescriptor;
}

GKSystem &
GKSystem::instance() {
    static GKSystemImp sys;
    return sys;
}

GKHotKeyManager &
GKHotKeyManager::instance() {
    static GKHotKeyManagerImp instance;
    return instance;
}

GKHotKeyManager::~GKHotKeyManager() = default;

void
GKHotKeyManager::loadHotKeys() {
    for (size_t i = 0; i < GKConfig::instance().appCount(); ++i) {
        GKPtr<GKHotKey> hotKey = createHotKey(GKConfig::instance().appKeySequence(i));
        hotKey->setHandler([i]() {
            GKToggleAppTask task(i);
            task();
        });
        hotKeys_.push_back(hotKey);
    }     
}

void
GKHotKeyManager::registerHotKeys() {
    for (auto & hotKey : hotKeys_)
        hotKey->registerHotKey();
}

void
GKHotKeyManager::unregisterHotKeys() {
    for (auto & hotKey : hotKeys_)
        hotKey->unregisterHotKey();
}

const std::vector<GKPtr<GKHotKey>> &
GKHotKeyManager::hotKeys() const {
    return hotKeys_;
}
