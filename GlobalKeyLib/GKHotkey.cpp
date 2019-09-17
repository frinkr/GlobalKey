#include <algorithm>
#include <sstream>
#include "GKHotkey.h"

#if GK_MAC
#  include "GKHotkeyMac.h"
#elif GK_WIN
#  include "GKHotkeyWin.h"
#endif


GKHotkey::GKHotkey(GKKeySequence keySequence)
    : keySequence_(std::move(keySequence))
{
#if GK_WIN
    imp_ = std::make_unique<Imp>(this, GKHotkeyTargetHWND);
#else
    imp_ = std::make_unique<Imp>(this);
#endif
}

GKHotkey::~GKHotkey() = default;

const GKKeySequence &
GKHotkey::keySequence() const {
    return keySequence_;
}

GKErr
GKHotkey::registerHotkey() {
    if (registered_)
        return GKErr::noErr;
    
    GKErr err = imp_->registerHotkey();
    registered_ = (err == GKErr::noErr);
    return err;
}

GKErr
GKHotkey::unregisterHotkey() {
    if (!registered_)
        return GKErr::noErr;
    
    GKErr err = imp_->unregisterHotkey();
    registered_ = (err == GKErr::hotkeyCantUnregisteer);
    return err;
}

bool
GKHotkey::isRegistered() const {
    return registered_;
}

void
GKHotkey::invoke() {
    if (handler_)
        handler_();
}

GKHotkey::Ref
GKHotkey::ref() const {
    return imp_->ref();
}

void
GKHotkey::setHandler(Handler handler) {
    handler_ = handler;
}

const GKHotkey::Handler&
GKHotkey::handler() const {
    return handler_;
}

std::optional<std::pair<GKHotkeyModifier, std::string> >
GKSplitKeySequence(const GKKeySequence & commandKeySequence) {
    std::underlying_type_t<GKHotkeyModifier> modifiers = 0;
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
    return std::make_pair(GKHotkeyModifier(modifiers), key);
}
