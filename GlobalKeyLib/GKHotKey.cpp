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
#else
    imp_ = std::make_unique<Imp>(this);
#endif
}

GKHotKey::~GKHotKey() = default;

const GKKeySequence &
GKHotKey::keySequence() const {
    return keySequence_;
}

GKErr
GKHotKey::registerHotKey() {
    if (registered_)
        return GKErr::noErr;
    
    GKErr err = imp_->registerHotKey();
    registered_ = (err == GKErr::noErr);
    return err;
}

GKErr
GKHotKey::unregisterHotKey() {
    if (!registered_)
        return GKErr::noErr;
    
    GKErr err = imp_->unregisterHotKey();
    registered_ = (err == GKErr::hotKeyCantUnregister);
    return err;
}

bool
GKHotKey::isRegistered() const {
    return registered_;
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

std::optional<std::pair<GKHotKeyModifier, std::string> >
GKSplitKeySequence(const GKKeySequence & commandKeySequence) {
    std::underlying_type_t<GKHotKeyModifier> modifiers = 0;
    std::string key;

    std::stringstream ss(commandKeySequence);
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
#if GK_WIN
        else if (item == "WIN")
            modifiers |= kMETA;
#elif GK_MAC
        else if (item == "CMD")
            modifiers |= kMETA;
#endif
        else if (key.empty())
            key = std::move(item);
        else
            return std::nullopt;
    }
    return std::make_pair(GKHotKeyModifier(modifiers), key);
}
