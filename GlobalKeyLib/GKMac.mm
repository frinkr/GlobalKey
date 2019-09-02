#import <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>
#include <array>
#include <string_view>
#include <stdexcept>
#include "GKMac.h"

namespace {
    NSString *
    fromStdString(const std::string & str) {
        return [NSString stringWithCString:str.c_str()
                                   encoding:[NSString defaultCStringEncoding]];
    }

    std::string
    toStdString(NSString * str) {
        return std::string([str UTF8String]);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//                             GKAppProxy::Imp:

struct GKAppProxy::Imp::MacImp {
public:
    MacImp(GKAppProxy * parent)
    : parent_(parent) {}
    
    NSRunningApplication *
    runningApp() const {
        NSString * path = fromStdString(parent_->descriptor());
        
        NSString * bundleIdentifier = nil;
        NSBundle * bundle = [NSBundle bundleWithPath:path];
        if (bundle)
            bundleIdentifier = [bundle bundleIdentifier];
        else
            bundleIdentifier = path;
        
        // Search bundle
        if (bundleIdentifier) {
            NSArray<NSRunningApplication *> * running = [NSRunningApplication runningApplicationsWithBundleIdentifier:bundleIdentifier];
            if ([running count])
                return [running objectAtIndex:0];
        }
        
        
        // Not a bundle, search by executable path
        NSWorkspace * workspace = [NSWorkspace sharedWorkspace];
        NSArray<NSRunningApplication *> * runningApps = [workspace runningApplications];
        for (NSUInteger i = 0; i < [runningApps count]; ++ i) {
            NSRunningApplication * app = [runningApps objectAtIndex:i];
            if ([app.executableURL.path isEqualToString:path])
                return app;
        }
        return nil;
    }
    
    static NSRunningApplication *
    frontmostApp() {
        NSWorkspace * workspace = [NSWorkspace sharedWorkspace];
        return [workspace frontmostApplication];
    }
private:
    GKAppProxy * parent_ {};
};

GKAppProxy::Imp::Imp(GKAppProxy * parent)
    : parent_(parent)
    , imp_(std::make_unique<GKAppProxy::Imp::MacImp>(parent))
    {}

GKAppProxy::Imp::~Imp() = default;


GKErr
GKAppProxy::Imp::bringFront() {
    @autoreleasepool {
        NSRunningApplication * app = imp_->runningApp();
        if (!app)
            return GKErr::appCantFound;
        
        // NSApplicationActivateAllWindows not work for Emacs.app on 10.15 Beta (19A546d)
        if ([app activateWithOptions:NSApplicationActivateIgnoringOtherApps])
            return GKErr::noErr;
        else
            return GKErr::appCantActivate;
    }
}

GKErr
GKAppProxy::Imp::show() {
    @autoreleasepool {
        NSRunningApplication * app = imp_->runningApp();
        if ([app unhide])
            return GKErr::noErr;
        else
            return GKErr::appCantFound;
    }
}

GKErr
GKAppProxy::Imp::hide() {
    @autoreleasepool {
        NSRunningApplication * app = imp_->runningApp();
        if ([app hide])
            return GKErr::noErr;
        else
            return GKErr::appCantFound;
    }
}

bool
GKAppProxy::Imp::visible() const  {
    @autoreleasepool {
        NSRunningApplication * app = imp_->runningApp();
        if (app)
            return not [app isHidden];
        else
            return false;
    }
}

bool
GKAppProxy::Imp::atFrontmost() const {
    @autoreleasepool {
        NSRunningApplication * app = imp_->runningApp();
        NSRunningApplication * front = MacImp::frontmostApp();
        return [[app bundleURL] isEqualTo:[front bundleURL]];
    }
}

bool
GKAppProxy::Imp::running() const  {
    @autoreleasepool {
        return nil != imp_->runningApp();
    }
}

GKErr
GKAppProxy::Imp::launch() {
    @autoreleasepool {
        NSWorkspace * workspace = [NSWorkspace sharedWorkspace];
        
        NSString * path = fromStdString(parent_->descriptor());
        NSURL * url = [workspace URLForApplicationWithBundleIdentifier:path];
        if (!url)
            url = [NSURL fileURLWithPath:path];
        
        BOOL ok = [workspace openURL:url];
        return ok? GKErr::noErr: GKErr::appCantLaunch;
    }
}


GKHotKey::Imp::Imp(GKHotKey * parent)
: parent_(parent) {}

void
GKHotKey::Imp::registerHotKey() {
    
}

void
GKHotKey::Imp::unregisterHotKey() {
    
}

GKHotKey::Ref
GKHotKey::Imp::ref() const {
    return nullptr;
}


//////////////////////////////////////////////////////////////////////////////////////////
//                            GKSystemImp

void
GKSystemImp::postNotification(const std::string & title, const std::string & message) {
    @autoreleasepool {
        NSUserNotification * notification = [[NSUserNotification alloc] init];
        notification.title = fromStdString(title);
        notification.informativeText = fromStdString(message);
        notification.soundName = NSUserNotificationDefaultSoundName;
        [[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification: notification];
    }
}
