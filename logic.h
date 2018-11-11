#ifndef LOGIC_SEEN
#define LOGIC_SEEN

#include "gba.h"

#define SWING_FRAME_COUNTER_START 10
#define SERVE_VELOCITY_START -2
#define GROUND SCREEN_HEIGHT - 20

#define NET_BOUNDARY(body_width) ((SCREEN_WIDTH) / 2 - (body_width)*2)

#define HIT_BOX_X(playerX, swingCounter) ((playerX) + 10 - 5 * ((swingCounter) / 3))
#define HIT_BOX_Y(playerY, swingCounter) ((playerY)-10 + 3 * (swingCounter) / 3)

#define APPLY_GRAVITY(velocity, vBlankCounter) (((vBlankCounter % 13) / 12) + velocity)

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
    int isCpu;
    HitBox racketHitBox;
    PlayerBody body;
} Player;

typedef struct
{
    int x;
    int y;
    int velX;
    int velY;
    int size;
    u16 color;
} Ball;

typedef struct
{
    // Store whether or not the game is over in this member:
    int gameOver;
    Player player;
    Player cpu;
    Ball ball;

    int playerServing;
    int cpuServing;
    int serveStarted;

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
