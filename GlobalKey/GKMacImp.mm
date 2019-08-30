#import <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>
#include <array>
#include <string_view>
#include <stdexcept>
#include "GKMacImp.h"

namespace {
    void throwBadObjectType() {
        throw std::runtime_error("Bad Object Type");
    }

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
//                            GKMacAppId

GKMacAppId::GKMacAppId(std::string path)
    : path_(std::move(path)) {
}

const std::string &
GKMacAppId::path() const {
    return path_;
}

std::string
GKMacAppId::description() const {
    return path_;
}


//////////////////////////////////////////////////////////////////////////////////////////
//                            GKMacApp

struct GKMacApp::Imp {
public:
    Imp(GKMacApp * parent)
    : parent_(parent) {}
    
    NSRunningApplication *
    runningApp() const {
        // Search the running app by bundle path
        NSString * path = fromStdString(parent_->id().path());
        NSWorkspace * workspace = [NSWorkspace sharedWorkspace];
        NSArray<NSRunningApplication *> * runningApps = [workspace runningApplications];
        for (NSUInteger i = 0; i < [runningApps count]; ++ i) {
            NSRunningApplication * app = [runningApps objectAtIndex:i];
            if ([app.bundleURL.path isEqualToString:path])
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
    GKMacApp * parent_ {};
};

GKMacApp::GKMacApp(const GKMacAppId & appId)
    : id_(appId)
    , imp_(std::make_unique<GKMacApp::Imp>(this))
    {}

GKMacApp::~GKMacApp() = default;

const GKMacAppId &
GKMacApp::id() const {
    return id_;
}

GKErr
GKMacApp::bringFront() {
    GKErr err = launch();
    
    if (!atFrontmost())
    {
        NSLog(@"Not at front");
    }
    return err;
    
#if 0
    @autoreleasepool {
        NSRunningApplication * app = imp_->runningApp();
        if (!app)
            return GKErr::appCantFound;
        
        if ([app activateWithOptions:NSApplicationActivateAllWindows])
            return GKErr::noErr;
        else
            return GKErr::appCantActivate;
    }
#endif
}

GKErr
GKMacApp::show() {
    @autoreleasepool {
        NSRunningApplication * app = imp_->runningApp();
        if ([app unhide])
            return GKErr::noErr;
        else
            return GKErr::appCantFound;
    }
}

GKErr
GKMacApp::hide() {
    @autoreleasepool {
        NSRunningApplication * app = imp_->runningApp();
        if ([app hide])
            return GKErr::noErr;
        else
            return GKErr::appCantFound;
    }
}

bool
GKMacApp::visible() const  {
    @autoreleasepool {
        NSRunningApplication * app = imp_->runningApp();
        if (app)
            return not [app isHidden];
        else
            return false;
    }
}

bool
GKMacApp::atFrontmost() const {
    @autoreleasepool {
        NSRunningApplication * app = imp_->runningApp();
        NSRunningApplication * front = Imp::frontmostApp();
        return [[app bundleURL] isEqualTo:[front bundleURL]];
    }
}

bool
GKMacApp::running() const  {
    @autoreleasepool {
        return nil != imp_->runningApp();
    }
}

GKErr
GKMacApp::launch() {
    @autoreleasepool {
        NSWorkspace * workspace = [NSWorkspace sharedWorkspace];
        NSURL * url = [NSURL fileURLWithPath:fromStdString(id_.path())];
        BOOL ok = [workspace openURL:url];
        //BOOL ok = [workspace launchApplication:fromStdString(id_.path())];
        return ok? GKErr::noErr: GKErr::appCantLaunch;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//                            GKMacAppFactory

GKMacAppFactory::GKMacAppFactory() {
}

GKPtr<GKApp>
GKMacAppFactory::getOrCreateApp(GKPtr<const GKAppId> appId) {
    auto id = std::dynamic_pointer_cast<const GKMacAppId>(appId);
    if (!id)
        throwBadObjectType();
    auto app = std::make_shared<GKMacApp>(*id);

    // TODO: cache
    return app;
}


//////////////////////////////////////////////////////////////////////////////////////////
//                            GKMacConfig

GKMacConfig::GKMacConfig(std::string file)
: file_(std::move(file)) {
    @autoreleasepool {
        NSString * path = fromStdString(file_);
        NSDictionary * dict = [NSDictionary dictionaryWithContentsOfFile:path];
        NSArray * apps = [dict objectForKey:@"apps"];
        for (NSUInteger i = 0; i < apps.count; ++ i) {
            NSDictionary * appDict = [apps objectAtIndex:i];
            
            Entry e;
            e.keySequence = toStdString([appDict objectForKey:@"key"]);
            e.bundlePath = toStdString([appDict objectForKey:@"app"]);
            entries_.push_back(e);
        }
    }
}

size_t
GKMacConfig::appCount() const {
    return entries_.size();
}

std::string
GKMacConfig::appKeySequence(size_t index) const {
    return entries_[index].keySequence;
}

GKPtr<const GKAppId>
GKMacConfig::appId(size_t index) const {
    return std::make_shared<GKMacAppId>(entries_[index].bundlePath);
}
