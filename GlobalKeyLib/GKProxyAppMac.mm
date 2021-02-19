#import <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>
#include <Carbon/Carbon.h>
#import <ApplicationServices/ApplicationServices.h>

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

    void CFValueCast_Imp(CFTypeRef ref, GKOpt<bool> & out) {
        if (CFGetTypeID(ref) == CFBooleanGetTypeID())
            out = CFBooleanGetValue((CFBooleanRef)ref);
    }
        
    template <typename T>
    GKOpt<T> CFValueCast(CFTypeRef ref) {
        GKOpt<T> val;
        CFValueCast_Imp(ref, val);
        return val;
    }
        
    namespace aux {
        template <typename T>
        GKOpt<T> getValue(AXUIElementRef element, CFStringRef attr) {
            CFTypeRef valueRef = nil;
            if (!AXUIElementCopyAttributeValue(element, attr, &valueRef) && valueRef) {
                GKOpt<T> value = CFValueCast<T>(valueRef);
                CFRelease(valueRef);
                return value;
            }
            return std::nullopt;
        }

        NSArray * getValues(AXUIElementRef element, CFStringRef attr) {
            NSMutableArray * values = nil;
            if (auto error = AXUIElementCopyAttributeValues(element, attr, 0, 100, (CFArrayRef *)&values); !error && values)
                return [values autorelease];
            else
                return [[NSMutableArray alloc] init];
        }
        
        template <typename T>
        T getValueWithDefault(AXUIElementRef element, CFStringRef attr, T defaultValue) {
            auto val = getValue<T>(element, attr);
            if (val)
                return *val;
            else
                return defaultValue;
        }
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
        
        
        //return GKErr::noErr;
        
//       NSRunningApplication.activateWithOptions doesn't restore a minimized app.
//
        // NSApplicationActivateAllWindows not work for Emacs.app on 10.15 Beta (19A546d)
        if ([app activateWithOptions:NSApplicationActivateIgnoringOtherApps]) {
            [[NSWorkspace sharedWorkspace] openApplicationAtURL:[app bundleURL] configuration:config completionHandler:nil];
            //[app unhide];
            return GKErr::noErr;
        }
        else
            return GKErr::appCantActivate;
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
        if (app && not [app isHidden]) {
            AXUIElementRef applicationRef = AXUIElementCreateApplication([app processIdentifier]);
            NSArray * windows = aux::getValues(applicationRef, kAXWindowsAttribute);
            bool minimized = false;
            NSEnumerator * windowItr = [windows objectEnumerator];
            while (AXUIElementRef windowRef = (__bridge AXUIElementRef)[windowItr nextObject]) {
                minimized = aux::getValueWithDefault<bool>(windowRef, kAXMinimizedAttribute, false);
                if (minimized)  // find one minimized window
                    break;
            }
            return !minimized;
        }
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
