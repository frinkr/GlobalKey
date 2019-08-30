#pragma once

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

class GKAppId {
public:
    virtual ~GKAppId() {};
    
    virtual std::string
    description() const = 0;
};

enum class GKErr : unsigned char{
    noErr             = 0,
    
    appCantFound      = 10,
    appCantLaunch     = 11,
    appCantActivate   = 12,
    
    notImplemented    = 99,
};

class GKApp {
public:
    virtual
    ~GKApp() {};

    /**
     * The application identifier
     */
    virtual const GKAppId &
    id() const = 0;

    /**
     * Bring the application front
     */
    virtual GKErr
    bringFront() = 0;

    /**
     * Show the application
     */
    virtual GKErr
    show() = 0;

    /*
     * Hide the application, i.e. minimize the application window
     */
    virtual GKErr
    hide() = 0;

    /**
     * If the application's window is visible
     */
    virtual bool
    visible() const = 0;

    /**
     * If application's window is at front (accept key)
     */
    virtual bool
    atFrontmost() const = 0;
    
    /**
     * Check if the application is running
     */
    virtual bool
    running() const = 0;


    /*
     * Launch the application if not running
     */
    virtual GKErr
    launch() = 0;
};

class GKAppFactory {
public:
    static GKAppFactory &
    instance();

    virtual ~GKAppFactory() {};
    
    virtual GKPtr<GKApp>
    getOrCreateApp(GKPtr<const GKAppId> appId) = 0;
};


class GKConfig {
public:
    virtual ~GKConfig() {}

    virtual std::string
    path() const = 0;

    virtual size_t
    appCount() const = 0;

    virtual std::string
    appKeySequence(size_t index) const = 0;
    
    virtual GKPtr<const GKAppId>
    appId(size_t index) const = 0;

    static const GKConfig &
    instance();
};


class GKSystem {
public:

    static GKSystem &
    instance();

    virtual ~GKSystem() {}

    virtual void
    postNotification(const std::string & title, const std::string & message) = 0;
};
