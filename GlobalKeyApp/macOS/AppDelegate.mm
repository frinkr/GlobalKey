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
}

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    std::cout << "ello" << std::endl;
    
    tray = [[NSStatusBar systemStatusBar] statusItemWithLength:NSSquareStatusItemLength];
    tray.button.title = @"🐙";
    
    //self.tray.button.image = [NSImage imageNamed:@"AppIcon"];
    
    NSMenu * trayMenu = [[NSMenu alloc] initWithTitle:@"Tray"];
    tray.menu = trayMenu;
    
    enableItem = [trayMenu addItemWithTitle:@"Enable" action:@selector(onEnableMenuItem:) keyEquivalent:@"e"] ;
    [enableItem setTarget:self];
    [[trayMenu addItemWithTitle:@"Reload" action:@selector(onReloadMenuItem:) keyEquivalent:@"r"] setTarget: self];
    [[trayMenu addItemWithTitle:@"Edit Shortcuts" action:@selector(onEditMenuItem:) keyEquivalent:@""] setTarget:self];
    [trayMenu addItem:[NSMenuItem separatorItem]];
    [[trayMenu addItemWithTitle:@"About" action:@selector(onAboutMenuItem:) keyEquivalent:@""] setTarget:self];
    [[trayMenu addItemWithTitle:@"Quit" action:@selector(quit:) keyEquivalent:@"q"] setTarget:self];
    
    [trayMenu setAutoenablesItems:FALSE];
    // Insert code here to initialize your application
    
    GKCoreApp::instance().reload(true);
    [self updateEnableMenuItemStatus:self];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

- (IBAction) updateEnableMenuItemStatus:(id)sender {
    if (GKCoreApp::instance().hotKeysRegistered()) {
        [enableItem setTitle:@"Disable"];
    }
    else {
        [enableItem setTitle:@"Enable"];
    }
    
}
- (IBAction) onEnableMenuItem:(id)sender {
    if (GKCoreApp::instance().hotKeysRegistered())
        GKCoreApp::instance().unregisterHotKeys();
    else
        GKCoreApp::instance().registerHotKeys();
    
    [self updateEnableMenuItemStatus:self];
}

- (IBAction) onReloadMenuItem:(id)sender {
    GKCoreApp::instance().reload(false);
}

- (IBAction) onEditMenuItem:(id)sender {
    GKSystem::postNotification("GlobalKey", "Edit");
}

- (IBAction) onAboutMenuItem:(id)sender {
    GKSystem::postNotification("GlobalKey", "About");
}

- (IBAction) quit:(id)sender {
    [NSApp terminate:self];
}
@end
