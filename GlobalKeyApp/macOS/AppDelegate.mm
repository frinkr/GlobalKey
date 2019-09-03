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
#include "GKSystem.h"

@interface AppDelegate ()
{
    NSStatusItem * tray;
    NSMenuItem * enableItem;
    NSMenuItem * reloadItem;
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
    
    [[trayMenu addItemWithTitle:@"Edit Shortcuts" action:@selector(onEditMenuItem:) keyEquivalent:@""] setTarget:self];
    [trayMenu addItem:[NSMenuItem separatorItem]];
    [[trayMenu addItemWithTitle:@"About" action:@selector(onAboutMenuItem:) keyEquivalent:@""] setTarget:self];
    [[trayMenu addItemWithTitle:@"Quit" action:@selector(quit:) keyEquivalent:@"q"] setTarget:self];
    
    GKCoreApp::instance().reload(true);
    [self syncGUI:self];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

- (IBAction) syncGUI:(id)sender {
    BOOL enabled = GKCoreApp::instance().hotKeysRegistered();
    
    // Update Icon
    float iconSize = [[NSStatusBar systemStatusBar] thickness] - 5;
    NSImage * trayIcon = [NSImage imageNamed:@"TrayIcon"];
    [trayIcon setSize:CGSizeMake(iconSize, iconSize)];
    if (!enabled) {
        trayIcon = [trayIcon copy];
        [trayIcon lockFocus];
        NSColor * tint = [NSColor colorWithCalibratedWhite:0.33 alpha:1];
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
}

- (IBAction) onEnableMenuItem:(id)sender {
    if (GKCoreApp::instance().hotKeysRegistered())
        GKCoreApp::instance().unregisterHotKeys();
    else
        GKCoreApp::instance().registerHotKeys();
    
    [self syncGUI:self];
}

- (IBAction) onReloadMenuItem:(id)sender {
    GKCoreApp::instance().reload(false);
}

- (IBAction) onEditMenuItem:(id)sender {
    GKCoreApp::instance().revealConfigFile();
}

- (IBAction) onAboutMenuItem:(id)sender {
    GKSystem::postNotification("GlobalKey", "About");
}

- (IBAction) quit:(id)sender {
    [NSApp terminate:self];
}
@end
