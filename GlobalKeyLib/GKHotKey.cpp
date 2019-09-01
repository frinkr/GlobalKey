#include <algorithm>
#include <sstream>
#include "GKHotKey.h"

#if GK_MAC
#  include "GKMac.h"
#elif GK_WIN
#  include "GKWin.h"
#endif


GKHotKey::GKHotKey(GKKeySequence keySequence)
    : keySequence_(std::move(keySequence))
{
#if GK_WIN
    imp_ = std::make_unique<Imp>(this, GKHotKeyTargetHWND);
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

GKHotKey::Ref
GKHotKey::ref() const {
    return imp_->ref();
}

void
GKHotKey::setHandler(Handler handler) {
    handler_ = handler;
}

const GKHotKey::Handler&
GKHotKey::handler() const {
    return handler_;
}

std::pair<GKHotKeyModifier, std::string>
GKSplitKeySequence(const GKKeySequence & keySequence) {
    std::underlying_type_t<GKHotKeyModifier> modifiers = 0;
    std::string key;

    std::stringstream ss(keySequence);
    std::string item;
    while (std::getline(ss, item, '+')) {
        std::transform(item.begin(), item.end(), item.begin(),
            [](unsigned char c) { return std::toupper(c); });

        if (item == "SHIFT")
            modifiers |= kSHIFT;
        else if (item == "CTRL")
            modifiers |= kCTRL;
        else if (item == "ALT")
            modifiers |= kALT;
        else if (item == "META")
            modifiers |= kMETA;
        else
            key = std::move(item);
    }
    return std::make_pair(GKHotKeyModifier(modifiers), key);
}