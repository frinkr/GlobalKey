#pragma once

#include "GK.h"

using GKAppDescriptor = std::string;

enum class GKErr : unsigned char {
    noErr = 0,

    appCantFound = 10,
    appCantLaunch = 11,
    appCantActivate = 12,
    appNotRunning = 13,

    notImplemented = 99,
};

class GKAppProxy : private GKNoCopy {
public:
    explicit GKAppProxy(const GKAppDescriptor& descriptor);

    virtual
    ~GKAppProxy();

    /**
     * The application identifier
     */
    virtual const GKAppDescriptor&
    descriptor() const;

    /**
     * Bring the application front
     */
    virtual GKErr
    bringFront();

    /**
     * Show the application
     */
    virtual GKErr
    show();

    /*
     * Hide the application, i.e. minimize the application window
     */
    virtual GKErr
    hide();

    /**
     * If the application's window is visible
     */
    virtual bool
    visible() const;

    /**
     * If application's window is at front (accept key)
     */
    virtual bool
    atFrontmost() const;

    /**
     * Check if the application is running
     */
    virtual bool
    running() const;


    /*
     * Launch the application if not running
     */
    virtual GKErr
    launch();

protected:
    class Imp;

    GKAppDescriptor descriptor_{};
    std::unique_ptr<Imp> imp_;
};