#include "GKProxyApp.h"

#if GK_WIN
#  include "GKWin.h"
#else
#  include "GKMac.h"
#endif

GKAppProxy::GKAppProxy(const GKAppDescriptor& descriptor)
    : descriptor_(descriptor)
    , imp_{ std::make_unique<Imp>(this) }{
}

GKAppProxy::~GKAppProxy() = default;

const GKAppDescriptor&
GKAppProxy::descriptor() const {
    return descriptor_;
}

GKErr
GKAppProxy::bringFront() {
    return imp_->bringFront();
}

GKErr
GKAppProxy::show() {
    return imp_->show();
}

GKErr
GKAppProxy::hide() {
    return imp_->hide();
}

bool
GKAppProxy::visible() const {
    return imp_->visible();
}

bool
GKAppProxy::atFrontmost() const {
    return imp_->atFrontmost();
}

bool
GKAppProxy::running() const {
    return imp_->running();
}

GKErr
GKAppProxy::launch() {
    return imp_->launch();
}
