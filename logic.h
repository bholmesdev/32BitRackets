#ifndef LOGIC_SEEN
#define LOGIC_SEEN

#include "gba.h"

#define NET_BOUNDARY(body_width) ((SCREEN_WIDTH) / 2 - (body_width)*2)

#define HIT_BOX_X(playerX, swingCounter) ((playerX) + 10 - 5 * ((swingCounter) / 5))
#define HIT_BOX_Y(playerY, swingCounter) ((playerY)-10 + 3 * (swingCounter) / 5)

typedef struct
{
    int x;
    int y;
    int size;
    int enabled;
    u16 debugColor;
} HitBox;

typedef struct
{
    int width;
    int height;
    u16 color;
} PlayerBody;

typedef struct
{
    int x;
    int y;
    int swingFrameCounter;
    HitBox racketHitBox;
    PlayerBody body;
} Player;

typedef struct
{
    // Store whether or not the game is over in this member:
    int gameOver;
    Player player;
    Player cpu;

    /*
    * TA-TODO: Add any logical elements you need to keep track of in your app.
    *
    * For example, for a Snake game, those could be:
    *
    * Snake snake;
    * Food foods[10];
    * int points;
    *
    */

} AppState;

/*
* TA-TODO: Add any additional structs that you need for your app.
*
* For example, for a Snake game, one could be:
*
* typedef struct {
*   int heading;
*   int length;
*   int x;
*   int y;
* } Snake;
*
*/

// This function can initialize an unused AppState struct.
void initializeAppState(AppState *appState);

// This function will be used to process app frames.
AppState processAppState(AppState *currentAppState, u32 keysPressedBefore, u32 keysPressedNow);

// If you have anything else you need accessible from outside the logic.c
// file, you can add them here. You likely won't.

#endif
