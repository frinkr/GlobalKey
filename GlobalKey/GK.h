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

using GKHandler = std::function<void()>;

using GKKeySequence = std::string;
using GKAppDescriptor = std::string;

enum class GKErr : unsigned char{
    noErr             = 0,
    
    appCantFound      = 10,
    appCantLaunch     = 11,
    appCantActivate   = 12,
    
    notImplemented    = 99,
};

class GKApp {
public:
    explicit GKApp(const GKAppDescriptor & descriptor);

    virtual
    ~GKApp() {};

    /**
     * The application identifier
     */
    virtual const GKAppDescriptor &
    descriptor() const;

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

protected:
    GKAppDescriptor descriptor_ {};
};

class GKAppFactory {
public:
    static GKAppFactory &
    instance();

    virtual ~GKAppFactory() {};
    
    virtual GKPtr<GKApp>
    getOrCreateApp(const GKAppDescriptor & appDescriptor) = 0;
};

class GKHotKey {
public:
    explicit GKHotKey(GKKeySequence keySequence)
    : keySequence_(std::move(keySequence)) {}

    virtual ~GKHotKey() {}

    virtual void
    registerHotKey() = 0;

    virtual void
    unregisterHotKey() = 0;

    virtual void
    invoke() {
        if (handler_)
            handler_();
    }

    void
    setHandler(GKHandler handler) {
        handler_ = handler;
    }

    const GKHandler &
    handler() const {
        return handler_;
    }

protected:
    GKHandler handler_;
    GKKeySequence keySequence_;
};


class GKHotKeyManager {
public:
    static GKHotKeyManager &
    instance();

    virtual ~GKHotKeyManager();

    virtual void
    loadHotKeys();

    virtual void
    registerHotKeys();

    virtual void
    unregisterHotKeys();

    const std::vector<GKPtr<GKHotKey>> &
    hotKeys() const;

    virtual GKPtr<GKHotKey>
    createHotKey(const GKKeySequence& keySequence) = 0;

private:
    std::vector<GKPtr<GKHotKey>> hotKeys_;
};


class GKConfig {
private:
    struct Entry {
        GKKeySequence     keySequence;
        GKAppDescriptor   appDescriptor;
    };

public:
    virtual ~GKConfig() {}

    virtual std::string
    path() const;

    virtual size_t
    appCount() const;

    virtual const GKKeySequence&
    appKeySequence(size_t index) const;
    
    virtual const GKAppDescriptor &
    appDescriptor(size_t index) const;

    static const GKConfig &
    instance();

protected:
    virtual void
    load() = 0;

protected:
    std::string file_;
    std::vector<Entry> entries_;
};

class GKSystem {
public:

    static GKSystem &
    instance();

    virtual ~GKSystem() {}

    virtual void
    postNotification(const std::string & title, const std::string & message) = 0;
};
