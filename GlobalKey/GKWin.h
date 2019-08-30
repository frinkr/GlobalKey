#pragma once

#include <Windows.h>
#include "GK.h"

class GKWinHotKey : public GKHotKey {
public:
    GKWinHotKey(HWND hwnd, const std::string & keySequence);

    void
    registerHotKey() override;

    void
    unregisterHotKey() override;

    int 
    id() const;
private:
    HWND hwnd_{};
    int modifiers_{};
    int virtualKey_{};
};