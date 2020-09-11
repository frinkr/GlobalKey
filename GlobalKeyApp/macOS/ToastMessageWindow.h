#import <Cocoa/Cocoa.h>

@interface ToastMessageView : NSView
@property NSString * message;
@property NSString * title;
@property NSString * icon;
@end

@interface ToastMessageWindow: NSWindow
+ (ToastMessageWindow*) sharedInstance;
- (void)postMessage:(NSString*)message withTitle:(NSString*)title andIcon:(NSString*)icon;
@end
