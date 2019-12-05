#import <Cocoa/Cocoa.h>

@interface ToastMessageView : NSView
@property NSString * message;
@property NSString * title;
@end

@interface ToastMessageWindow: NSWindow
+ (ToastMessageWindow*) sharedInstance;
- (void)postMessage:(NSString*)message withTitle:(NSString*)title;
@end
