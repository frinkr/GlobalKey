//
//  AppDelegate.m
//  app
//
//  Created by cici on 2019/9/2.
//  Copyright © 2019 cici. All rights reserved.
//

#import "AppDelegate.h"
#include <iostream>
#include "../GKCoreApp.h"
#include "../GKSystemService.h"
#import "ToastMessageWindow.h"
@interface AppDelegate ()
{
    NSStatusItem * tray;
    NSMenuItem * enableItem;
    NSMenuItem * reloadItem;
    NSMenuItem * caffItem;
}

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    tray = [[NSStatusBar systemStatusBar] statusItemWithLength:NSSquareStatusItemLength];
    
    NSMenu * trayMenu = [[NSMenu alloc] initWithTitle:@"Tray"];
    [trayMenu setAutoenablesItems:FALSE];
    
    tray.menu = trayMenu;
    
    enableItem = [trayMenu addItemWithTitle:@"Enable" action:@selector(onEnableMenuItem:) keyEquivalent:@"e"] ;
    [enableItem setTarget:self];
    reloadItem = [trayMenu addItemWithTitle:@"Reload" action:@selector(onReloadMenuItem:) keyEquivalent:@"r"];
    [reloadItem setTarget:self];
    [[trayMenu addItemWithTitle:@"Edit Hotkeys" action:@selector(onEditMenuItem:) keyEquivalent:@""] setTarget:self];

    [trayMenu addItem:[NSMenuItem separatorItem]];

    caffItem = [trayMenu addItemWithTitle:@"Caffinate" action:@selector(onCaffinateMenuItem:) keyEquivalent:@""];
    [caffItem setTarget:self];
    
    [trayMenu addItem:[NSMenuItem separatorItem]];

    [[trayMenu addItemWithTitle:@"About" action:@selector(onAboutMenuItem:) keyEquivalent:@""] setTarget:self];
    [[trayMenu addItemWithTitle:@"Quit" action:@selector(quit:) keyEquivalent:@"q"] setTarget:self];
    
    gkApp.reload(true);
    [self syncGUI:self];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

- (IBAction) syncGUI:(id)sender {
    BOOL enabled = gkApp.hotkeysRegistered();
    
    // Update Icon
    float iconSize = [[NSStatusBar systemStatusBar] thickness] - 5;
    NSImage * trayIcon = [NSImage imageNamed:GKSystemService::isKeepingComputerAwake()? @"Caffee": @"TrayIcon"];
    [trayIcon setSize:CGSizeMake(iconSize, iconSize)];
    if (!GKSystemService::isKeepingComputerAwake() && !enabled) {
        trayIcon = [trayIcon copy];
        [trayIcon lockFocus];
        NSColor * tint = [NSColor colorWithCalibratedWhite:0.0 alpha:0.618];
        [tint set];
        NSRect iconRect = {NSZeroPoint, [trayIcon size]};
        NSRectFillUsingOperation(iconRect, NSCompositingOperationSourceAtop);
        [trayIcon unlockFocus];
        [trayIcon setTemplate:FALSE];
    }
    tray.button.image = trayIcon;
    
    // Update menu text
    if (enabled)
        [enableItem setTitle:@"Disable"];
    else
        [enableItem setTitle:@"Enable"];
    
    // Enable/Disable 'Reload'
    [reloadItem setEnabled:enabled];

    // Caffinate
    if (GKSystemService::isKeepingComputerAwake())
        caffItem.state = NSControlStateValueOn;
    else
        caffItem.state = NSControlStateValueOff;
}

- (IBAction) onEnableMenuItem:(id)sender {
    if (gkApp.hotkeysRegistered())
        gkApp.unregisterHotkeys();
    else
        gkApp.registerHotkeys();
    
    [self syncGUI:self];
}

- (IBAction) onReloadMenuItem:(id)sender {
    gkApp.reload(false);
}

- (IBAction) onEditMenuItem:(id)sender {
    gkApp.revealConfigFile();
}

- (IBAction) onCaffinateMenuItem:(id)sender {
    if (GKSystemService::isKeepingComputerAwake())
        GKSystemService::stopKeepingComputerAwake();
    else
        GKSystemService::keepComputerAwake();

    [self syncGUI:self];
}

- (IBAction) onAboutMenuItem:(id)sender {
    GKSystemService::openUrl(GKAPP_URL);
}

- (IBAction) quit:(id)sender {
    [NSApp terminate:self];
}

- (void)postMessage:(NSString *)message withTitle:(NSString *)title andIcon:(NSString *)icon{
    [[ToastMessageWindow sharedInstance] postMessage:message withTitle:title andIcon:icon];
}

@end
