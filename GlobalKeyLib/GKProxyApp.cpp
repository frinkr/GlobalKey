#include "GKProxyApp.h"

#if GK_WIN
#  include "GKProxyAppWin.h"
#else
#  include "GKProxyAppMac.h"
#endif

GKProxyApp::GKProxyApp(const GKAppDescriptor& descriptor)
    : descriptor_(descriptor)
    , imp_{ std::make_unique<Imp>(this) }{
}

GKProxyApp::~GKProxyApp() = default;

const GKAppDescriptor&
GKProxyApp::descriptor() const {
    return descriptor_;
}

GKErr
GKProxyApp::bringFront() {
    return imp_->bringFront();
}

GKErr
GKProxyApp::show() {
    return imp_->show();
}

GKErr
GKProxyApp::hide() {
    return imp_->hide();
}

bool
GKProxyApp::visible() const {
    return imp_->visible();
}

bool
GKProxyApp::atFrontmost() const {
    return imp_->atFrontmost();
}

bool
GKProxyApp::running() const {
    return imp_->running();
}

GKErr
GKProxyApp::launch() {
    return imp_->launch();
}
