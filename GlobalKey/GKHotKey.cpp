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

