#include <iostream>
#include "../GKAppConfig.h"
#include "../GKAppHotKeyManager.h"

int main() {
    
    std::cout << "hello" << std::endl;
    GKAppHotKeyManager::instance().loadHotKeys();
    return 0;
}
