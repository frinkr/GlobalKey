
#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"

int main(int argc, const char * argv[]) {
    [[NSApplication sharedApplication] setDelegate:[[AppDelegate alloc] init]];
    [[NSApplication sharedApplication] run];
    //return NSApplicationMain(argc, argv);
    return 0;
}
