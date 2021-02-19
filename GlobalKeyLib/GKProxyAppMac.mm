#import <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>
#include <Carbon/Carbon.h>

#include <array>
#include <string_view>
#include <stdexcept>
#include <map>
#include "GKProxyAppMac.h"

namespace {
    NSString *
    fromStdString(const std::string & str) {
        return [NSString stringWithCString:str.c_str()
                                  encoding:NSUTF8StringEncoding];
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//                             GKProxyApp::Imp:

struct GKProxyApp::Imp::MacImp {
public:
    MacImp(GKProxyApp * parent)
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
    GKProxyApp * parent_ {};
};

GKProxyApp::Imp::Imp(GKProxyApp * parent)
    : parent_(parent)
    , imp_(std::make_unique<GKProxyApp::Imp::MacImp>(parent))
    {}

GKProxyApp::Imp::~Imp() = default;


GKErr
GKProxyApp::Imp::bringFront() {
    @autoreleasepool {
        NSRunningApplication * app = imp_->runningApp();
        if (!app)
            return GKErr::appCantFound;
        
        NSWorkspaceOpenConfiguration * config = [NSWorkspaceOpenConfiguration configuration];
        config.allowsRunningApplicationSubstitution = FALSE;
        
        [[NSWorkspace sharedWorkspace] openApplicationAtURL:[app bundleURL] configuration:config completionHandler:nil];
        return GKErr::noErr;
        
//       NSRunningApplication.activateWithOptions doesn't restore a minimized app.
//
//        // NSApplicationActivateAllWindows not work for Emacs.app on 10.15 Beta (19A546d)
//        if ([app activateWithOptions:NSApplicationActivateIgnoringOtherApps | NSApplicationActivateAllWindows]) {
//            //[app unhide];
//            return GKErr::noErr;
//        }
//        else
//            return GKErr::appCantActivate;
    }
}

GKErr
GKProxyApp::Imp::show() {
    @autoreleasepool {
        NSRunningApplication * app = imp_->runningApp();
        if ([app unhide])
            return GKErr::noErr;
        else
            return GKErr::appCantFound;
    }
}

GKErr
GKProxyApp::Imp::hide() {
    @autoreleasepool {
        NSRunningApplication * app = imp_->runningApp();
        if ([app hide])
            return GKErr::noErr;
        else
            return GKErr::appCantFound;
    }
}

bool
GKProxyApp::Imp::visible() const  {
    @autoreleasepool {
        NSRunningApplication * app = imp_->runningApp();
        if (app)
            return not [app isHidden];
        else
            return false;
    }
}

bool
GKProxyApp::Imp::atFrontmost() const {
    @autoreleasepool {
        NSRunningApplication * app = imp_->runningApp();
        NSRunningApplication * front = MacImp::frontmostApp();
        return [[app bundleURL] isEqualTo:[front bundleURL]];
    }
}

bool
GKProxyApp::Imp::running() const  {
    @autoreleasepool {
        return nil != imp_->runningApp();
    }
}

GKErr
GKProxyApp::Imp::launch() {
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
