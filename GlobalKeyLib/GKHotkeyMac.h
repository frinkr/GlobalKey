#pragma once
#include <cstdint>
#include "GKHotkey.h"

class GKHotkey::Imp {
public:
    explicit Imp(GKHotkey * parent);
    
    GKErr
    registerHotkey();
    
    GKErr
    unregisterHotkey();
    
    GKHotkey::Ref
    ref() const;
    
private:
    GKHotkey * parent_{};
    
    GKHotkey::Ref ref_ {};
    uint32_t key_ {};  // -1 means invalid
    uint32_t mod_ {};
};

