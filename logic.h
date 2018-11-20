#ifndef LOGIC_SEEN
#define LOGIC_SEEN

#include "gba.h"
#include <stdlib.h>
#include <stdio.h>

#define PLAYER_HEIGHT 20
#define PLAYER_WIDTH 20
#define BALL_SIZE 4
#define COURT_EDGE_LEFT 8
#define COURT_EDGE_RIGHT (SCREEN_WIDTH) - 8 - BALL_SIZE
#define MATCH_LENGTH 3

#define SWING_FRAME_COUNTER_START 12
#define SERVE_VELOCITY_START -2
#define GROUND SCREEN_HEIGHT - 25
#define GRAVITY_FACTOR 13
#define SWING_DELAY_MIN 0

#define NET_BOUNDARY_LEFT ((SCREEN_WIDTH) / 2 - 2 * (BALL_SIZE))
#define NET_BOUNDARY_RIGHT ((SCREEN_WIDTH) / 2 + (BALL_SIZE))

#define HIT_BOX_X(playerX, swingCounter) ((playerX) + 10 - (swingCounter))
#define HIT_BOX_Y(playerY, swingCounter) ((playerY)-10 + (swingCounter))

#define HIT_BOX_X_CPU(playerX, swingCounter) ((playerX)-5 + (swingCounter))

#define BALL_VEL_X(hitBoxX, playerX) (((hitBoxX) - (playerX)) / 4 + 1)
#define BALL_VEL_Y(hitBoxY, playerY) (((playerY) - (hitBoxY)) / 4 - 3)

#define BALL_CPU_VEL_X(hitBoxX, playerX) (((hitBoxX) - (playerX)-4) / 4 - 1)

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
    int x;
    int y;
    int width;
    int height;
    int velJump;
    int jumpGravityCounter;
    int swingFrameCounter;
    int isCpu;
    HitBox racketHitBox;
} Player;

typedef struct
{
    int x;
    int y;
    int velX;
    int velY;
    int size;
    int hasBounced;
    int expectedLandingX;
    u16 landingDebugColor;
} Ball;

typedef struct
{
    int player;
    int cpu;
    int setsCompleted;
    u16 setWinsByColor[MATCH_LENGTH];
} Score;

typedef struct
{
    char *setsCompleted[MATCH_LENGTH];
    u16 setWinsByColor[MATCH_LENGTH];
} MatchStandings;

typedef struct TextDisplay
{
    char *text;
    int durationCounter;
    u16 color;
    MatchStandings matchStandings;
    struct TextDisplay *next;
} TextDisplay;

typedef struct
{
    int gameOver;
    int playerMatchWinner;
    int cpuMatchWinner;
    Player player;
    Player cpu;
    Ball ball;
    Score score;
    TextDisplay *textDisplayQueue;

    int playerServing;
    int serveStarted;

    int cpuSwingDelay;

} AppState;

// This function can initialize an unused AppState struct.
void initializeAppState(AppState *appState);

// This function will be used to process app frames.
AppState processAppState(AppState *currentAppState, u32 keysPressedBefore, u32 keysPressedNow);

// If you have anything else you need accessible from outside the logic.c
// file, you can add them here. You likely won't.

#endif
