#include "GKHotKey.h"
#include "GKTask.h"

#if GK_MAC
#  include "GKMac.h"
#elif GK_WIN
#  include "GKWin.h"
#endif


GKHotKey::GKHotKey(GKKeySequence keySequence)
    : keySequence_(std::move(keySequence))
{
#if GK_WIN
    imp_ = std::make_unique<Imp>(this, GKHotKeyCreationHWND);
#endif
}

GKHotKey::~GKHotKey() = default;

void
GKHotKey::registerHotKey() {
    return imp_->registerHotKey();
}

void
GKHotKey::unregisterHotKey() {
    return imp_->unregisterHotKey();
}

void
GKHotKey::invoke() {
    if (handler_)
        handler_();
}

int
GKHotKey::id() const {
    return imp_->id();
}

void
GKHotKey::setHandler(Handler handler) {
    handler_ = handler;
}

const GKHotKey::Handler&
GKHotKey::handler() const {
    return handler_;
}

GKHotKeyManager&
GKHotKeyManager::instance() {
    static GKHotKeyManager instance;
    return instance;
}

GKHotKeyManager::~GKHotKeyManager() = default;

void
GKHotKeyManager::loadHotKeys() {
    for (size_t i = 0; i < GKConfig::instance().appCount(); ++i) {
        GKPtr<GKHotKey> hotKey = std::make_shared<GKHotKey>(GKConfig::instance().appKeySequence(i));
        hotKey->setHandler([i]() {
            GKToggleAppTask task(i);
            task.run();
            });
        hotKeys_.push_back(hotKey);
    }
}

void
GKHotKeyManager::registerHotKeys() {
    for (auto& hotKey : hotKeys_)
        hotKey->registerHotKey();
}

void
GKHotKeyManager::unregisterHotKeys() {
    for (auto& hotKey : hotKeys_)
        hotKey->unregisterHotKey();
}

const std::vector<GKPtr<GKHotKey>>&
GKHotKeyManager::hotKeys() const {
    return hotKeys_;
}
