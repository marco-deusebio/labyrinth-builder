//
//  AppDelegate.m
//  Labyrinth Forge App
//
//  Created by Marco D’Eusebio on 6/26/26.
//

#import "AppDelegate.h"

#include "../Labyrinth Forge/Maze.hpp"
#include "../Labyrinth Forge/Player.hpp"
#include "../Labyrinth Forge/SfmlRenderer.hpp"

@interface AppDelegate ()

@property (strong) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    Maze maze(15, 9);
    Player player(1, 0);
    SfmlRenderer renderer;

    while (renderer.isOpen()) {
        @autoreleasepool {
            renderer.processEvents(maze, player);
            renderer.render(maze, player);
        }
    }

    [NSApp terminate:nil];
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


- (BOOL)applicationSupportsSecureRestorableState:(NSApplication *)app {
    return YES;
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
}


@end
