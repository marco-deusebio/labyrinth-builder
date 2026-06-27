//
//  AppDelegate.m
//  Labyrinth Builder App
//
//  Created by Marco D’Eusebio on 6/26/26.
//

#import "AppDelegate.h"

#include "../Labyrinth Builder/LabyrinthProgression.hpp"
#include "../Labyrinth Builder/Maze.hpp"
#include "../Labyrinth Builder/Player.hpp"
#include "../Labyrinth Builder/SfmlRenderer.hpp"

@interface AppDelegate ()

@property (strong) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    labyrinth::LabyrinthProgressionSettings progressionSettings;
    int labyrinthLevel = 1;
    Maze maze = labyrinth::createMazeForLevel(
        labyrinthLevel,
        progressionSettings
    );
    Player player = labyrinth::createPlayerAtEntrance(progressionSettings);
    SfmlRenderer renderer;
    
    renderer.setLabyrinthLevel(labyrinthLevel);

    while (renderer.isOpen()) {
        @autoreleasepool {
            renderer.processEvents(maze, player);
            renderer.render(maze, player);
            
            if (maze.isExit(player.getRow(), player.getColumn())) {
                ++labyrinthLevel;
                maze = labyrinth::createMazeForLevel(
                    labyrinthLevel,
                    progressionSettings
                );
                player = labyrinth::createPlayerAtEntrance(
                    progressionSettings
                );
                renderer.setLabyrinthLevel(labyrinthLevel);
            }
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
