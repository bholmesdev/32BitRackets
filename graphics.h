#ifndef GRAPHICS_SEEN
#define GRAPHICS_SEEN

#include "logic.h"
#include "gba.h"

#define BACKGROUND_COLOR (u16) COLOR(143, 238, 255)
#define MATCH_STANDINGS_X_OFFSET(scoreIndex) ((SCREEN_WIDTH) / 2 - (1 - (scoreIndex)) * 25 - 4)

// This function will be used to draw everything about the state of your app
// including the background and whatnot.
void fullDrawAppState(AppState *state);

// This function will be used to undraw (i.e. erase) things that might
// move in a frame. E.g. in a Snake game, erase the Snake, the food & the score.
void undrawAppState(AppState *state);

// This function will be used to draw things that might have moved in a frame.
// For example, in a Snake game, draw the snake, the food, the score.
void drawAppState(AppState *state);

// If you have anything else you need accessible from outside the graphics.c
// file, you can add them here. You likely won't.

#endif
