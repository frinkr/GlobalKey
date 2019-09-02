//
//  AppDelegate.m
//  app
//
//  Created by cici on 2019/9/2.
//  Copyright © 2019 cici. All rights reserved.
//

#import "AppDelegate.h"
#include <iostream>
#include "../GKAppConfig.h"
#include "../GKAppHotKeyManager.h"
#include "GKSystem.h"

@interface AppDelegate ()

@property NSStatusItem * tray;

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    std::cout << "ello" << std::endl;
    
    self.tray = [[NSStatusBar systemStatusBar] statusItemWithLength:NSSquareStatusItemLength];
    self.tray.button.title = @"🐙";
    
    NSMenu * trayMenu = [[NSMenu alloc] initWithTitle:@"Tray"];
    self.tray.menu = trayMenu;
    
    [[trayMenu addItemWithTitle:@"Enable" action:@selector(onEnableMenuItem:) keyEquivalent:@"e"] setTarget:self];
    [[trayMenu addItemWithTitle:@"Reload" action:@selector(onReloadMenuItem:) keyEquivalent:@"r"] setTarget: self];
    [[trayMenu addItemWithTitle:@"Edit Shortcuts" action:@selector(onEditMenuItem:) keyEquivalent:@""] setTarget:self];
    [trayMenu addItem:[NSMenuItem separatorItem]];
    [[trayMenu addItemWithTitle:@"About" action:@selector(onAboutMenuItem:) keyEquivalent:@""] setTarget:self];
    [[trayMenu addItemWithTitle:@"Quit" action:@selector(quit:) keyEquivalent:@"q"] setTarget:self];
    
    [trayMenu setAutoenablesItems:FALSE];
    // Insert code here to initialize your application
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
    
    std::cout << "word" << std::endl;
}



- (BOOL) validateMenuItem:(NSMenuItem *)menuItem {
    return YES;
}

- (IBAction) onEnableMenuItem:(id)sender {
    GKSystem::postNotification("GlobalKey", "Enable");
}

- (IBAction) onReloadMenuItem:(id)sender {
    GKSystem::postNotification("GlobalKey", "Reload");
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
