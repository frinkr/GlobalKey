#import "ToastMessageWindow.h"
#import <QuartzCore/QuartzCore.h>

@implementation ToastMessageView

- (void)drawRect:(NSRect)rect
{
    NSColor *bgColor = [NSColor colorWithCalibratedWhite:0.0 alpha:0.35];
    NSRect bgRect = rect;
    int minX = NSMinX(bgRect);
    int midX = NSMidX(bgRect);
    int maxX = NSMaxX(bgRect);
    int minY = NSMinY(bgRect);
    int midY = NSMidY(bgRect);
    int maxY = NSMaxY(bgRect);
    float radius = 25.0; // correct value to duplicate Panther's App Switcher
    NSBezierPath *bgPath = [NSBezierPath bezierPath];
    
    // Bottom edge and bottom-right curve
    [bgPath moveToPoint:NSMakePoint(midX, minY)];
    [bgPath appendBezierPathWithArcFromPoint:NSMakePoint(maxX, minY) 
                                     toPoint:NSMakePoint(maxX, midY) 
                                      radius:radius];
    
    // Right edge and top-right curve
    [bgPath appendBezierPathWithArcFromPoint:NSMakePoint(maxX, maxY) 
                                     toPoint:NSMakePoint(midX, maxY) 
                                      radius:radius];
    
    // Top edge and top-left curve
    [bgPath appendBezierPathWithArcFromPoint:NSMakePoint(minX, maxY) 
                                     toPoint:NSMakePoint(minX, midY) 
                                      radius:radius];
    
    // Left edge and bottom-left curve
    [bgPath appendBezierPathWithArcFromPoint:bgRect.origin 
                                     toPoint:NSMakePoint(midX, minY) 
                                      radius:radius];
    [bgPath closePath];
    
    [bgColor set];
    [bgPath fill];
    
    NSImage * trayIcon = [NSImage imageNamed:self.icon];
    if (!trayIcon)
        trayIcon = [NSImage imageNamed: @"AppIcon"];
    CGFloat trayIconWidth = (rect.size.width * 0.65) * (trayIcon.size.width > trayIcon.size.height? 1: (trayIcon.size.width/trayIcon.size.height));
    NSSize trayIconSize = NSMakeSize(trayIconWidth, trayIcon.size.height/trayIcon.size.width * trayIconWidth);
    NSRect iconRect = NSMakeRect(rect.origin.x + (rect.size.width - trayIconSize.width) / 2,
                                 rect.origin.y + (rect.size.height * 0.9 - trayIconSize.height),
                                 trayIconSize.width,
                                 trayIconSize.height);
    [trayIcon drawInRect: iconRect];
    
    if (self.message && self.title) {
        NSMutableParagraphStyle * paragraphStyle = [[NSParagraphStyle defaultParagraphStyle] mutableCopy];
        [paragraphStyle setAlignment:NSTextAlignmentCenter];
        NSMutableDictionary * attributes = [[NSMutableDictionary alloc] init];
        [attributes setValue:paragraphStyle forKey:NSParagraphStyleAttributeName];
        [attributes setValue:[NSColor whiteColor] forKey:NSForegroundColorAttributeName];
        [attributes setValue:[NSFont systemFontOfSize: 24] forKey:NSFontAttributeName];
        
        NSSize messageSize = [self.message sizeWithAttributes:attributes];
        
        [self.message drawInRect:NSMakeRect(rect.origin.x,
                                            (iconRect.origin.y + rect.origin.y - messageSize.height) / 2,
                                            rect.size.width, messageSize.height)
                  withAttributes:attributes];
    }
}

@end

@interface ToastMessageWindow()
{
    NSPoint initialLocation;
    
    NSTimer * autoFadeoutTimer;
}

@property ToastMessageView * messageView;
@end

@implementation ToastMessageWindow
- (id)initWithContentRect:(NSRect)contentRect 
                styleMask:(NSWindowStyleMask)aStyle
                  backing:(NSBackingStoreType)bufferingType 
                    defer:(BOOL)flag {
    if (self = [super initWithContentRect:contentRect 
                                styleMask:NSWindowStyleMaskBorderless | aStyle
                                  backing:bufferingType
                                    defer:flag])
    {
        [self setLevel: NSStatusWindowLevel];
        [self setBackgroundColor: [NSColor clearColor]];
        [self setAlphaValue:1.0];
        [self setOpaque:NO];
        [self setHasShadow:NO];
        [self setIgnoresMouseEvents:YES];
        return self;
    }
    
    return nil;
}


- (BOOL) canBecomeKeyWindow
{
    return NO;
}

+(ToastMessageWindow*) sharedInstance {
    static ToastMessageWindow * instance = NULL;
    if (instance == NULL) {
        NSRect rect = NSMakeRect(0, 0, 200, 200);
        instance = [[ToastMessageWindow alloc]
                       initWithContentRect:rect
                                 styleMask:NSWindowStyleMaskBorderless
                                   backing:NSBackingStoreBuffered
                                     defer:NO];
        NSView * contentView = [[NSView alloc] initWithFrame:rect];
        instance.messageView = [[ToastMessageView alloc] initWithFrame:rect];
        [contentView addSubview:instance.messageView];
        [instance setContentView:contentView];
    }
    return instance;
}

- (void)blurView:(NSView *)view
{
    NSView *blurView = [[NSView alloc] initWithFrame:view.bounds];
    blurView.wantsLayer = YES;
    blurView.layer.backgroundColor = [NSColor clearColor].CGColor;
    blurView.layer.masksToBounds = YES;
    blurView.layerUsesCoreImageFilters = YES;
    blurView.layer.needsDisplayOnBoundsChange = YES;

    CIFilter *saturationFilter = [CIFilter filterWithName:@"CIColorControls"];
    [saturationFilter setDefaults];
    [saturationFilter setValue:@2.0 forKey:@"inputSaturation"];

    CIFilter *blurFilter = [CIFilter filterWithName:@"CIGaussianBlur"]; // Other blur types are available
    [blurFilter setDefaults];
    [blurFilter setValue:@2.0 forKey:@"inputRadius"];

    blurView.layer.backgroundFilters = @[saturationFilter, blurFilter];

    [view addSubview:blurView];

    [blurView.layer setNeedsDisplay];
}

- (void)postMessage:(NSString*)message withTitle:(NSString*)title andIcon:(NSString*)icon{
    [autoFadeoutTimer invalidate];
    
    // set view message
    self.messageView.message = message;
    self.messageView.title = title;
    self.messageView.icon = icon;
    [self.messageView setNeedsDisplay:TRUE];
    
    // move to screen bottom center
    NSScreen * screen = [NSScreen mainScreen];
    NSRect rect = screen.visibleFrame;
    NSPoint center = NSMakePoint(rect.origin.x + rect.size.width / 2, rect.origin.y + rect.size.height * 0.2);
    NSPoint topLeft = NSMakePoint(center.x - self.frame.size.width / 2, center.y + self.frame.size.height / 2);
    [self setFrameTopLeftPoint:topLeft];
    [self makeKeyAndOrderFront:NSApp];
    
    autoFadeoutTimer = [NSTimer scheduledTimerWithTimeInterval:3.0 repeats:NO block:^(NSTimer * _Nonnull timer) {
        [self orderOut:self];
    }];
}
@end
