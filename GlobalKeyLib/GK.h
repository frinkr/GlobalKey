#pragma once

#include <functional>
#include <memory>
#include <string>

#if defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
#  define GK_WIN 1
#elif defined(__linux__)
#  define GK_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
#  define GK_MAC 1
#elif defined(unix) || defined(__unix__) || defined(__unix)
#  define GK_UNIX 1
#endif

template <typename T>
using GKPtr = std::shared_ptr<T>;

enum class GKErr : unsigned char {
    noErr = 0,

    appCantFound = 10,
    appCantLaunch = 11,
    appCantActivate = 12,
    appNotRunning = 13,

    hotKeySequenceNotValid = 30,
    hotKeyCantRegister = 31,
    hotKeyCantUnregisteer = 32,
    hotKeyExists = 33,
    
    
    notImplemented = 99,
};

class GKNoCopy {
public:
    GKNoCopy() = default;
protected:
    GKNoCopy(const GKNoCopy&) = delete;
    GKNoCopy& operator=(const GKNoCopy&) = delete;
};

