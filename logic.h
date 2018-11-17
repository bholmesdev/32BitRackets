#ifndef LOGIC_SEEN
#define LOGIC_SEEN

#include "gba.h"

#define PLAYER_HEIGHT 20
#define PLAYER_WIDTH 20

#define SWING_FRAME_COUNTER_START 12
#define SERVE_VELOCITY_START -2
#define GROUND SCREEN_HEIGHT - 20
#define GRAVITY_FACTOR 13
#define SWING_DELAY_MIN 0

#define NET_BOUNDARY(body_width) ((SCREEN_WIDTH) / 2 - (body_width)*2)

#define HIT_BOX_X(playerX, swingCounter) ((playerX) + 10 - 3 * (swingCounter) / 3)
#define HIT_BOX_Y(playerY, swingCounter) ((playerY)-10 + 3 * (swingCounter) / 3)

#define HIT_BOX_X_CPU(playerX, swingCounter) ((playerX)-10 + 3 * (swingCounter) / 3)

#define BALL_VEL_X(hitBoxX, playerX) (((hitBoxX) - (playerX)) / 4 + 1)
#define BALL_VEL_Y(hitBoxY, playerY) (((playerY) - (hitBoxY)) / 4 - 3)

#define BALL_CPU_VEL_X(hitBoxX, playerX) (((hitBoxX) - (playerX)) / 4 - 1)
#define BALL_CPU_VEL_Y(hitBoxY, playerY) (((playerY) - (hitBoxY)) / 4 + 3)

#define APPLY_BALL_GRAVITY(velocity, gravityCounter) ((((gravityCounter) % (GRAVITY_FACTOR)) / (GRAVITY_FACTOR - 1)) + (velocity))
#define APPLY_JUMP_GRAVITY(velocity, gravityCounter) ((((gravityCounter) % 7) / 6) + (velocity))

#define PLAYER_JUMP_CPU_POS_FACTOR(playerY, velocity) ((((GROUND) - (playerY)-PLAYER_HEIGHT) * ((GROUND) - (playerY)-PLAYER_HEIGHT)) / (17 - (velocity) * (velocity)))
#define BALL_SPEED_CPU_POS_FACTOR(velocity) ((velocity) * (velocity) * (velocity)*2)

#define RANDOMIZED_SWING_TIMING(vBlankCounter, ballVelocity) (((vBlankCounter) % (ballVelocity * 15)) + (SWING_DELAY_MIN) + 1)

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
    int velJump;
    int jumpGravityCounter;
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

    int expectedBallLandingX;

    int playerServing;
    int cpuServing;
    int serveStarted;

    int cpuSwingDelay;

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
