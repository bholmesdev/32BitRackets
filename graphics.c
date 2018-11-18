#include "graphics.h"
#include "gba.h"
#include "sprites.h"
#include "./images/tennisCourtBackground.h"
#include <stdio.h>

volatile OamEntry *playerSprite = &shadow[0];
volatile OamEntry *cpuSprite = &shadow[1];
volatile OamEntry *ballSprite = &shadow[2];

void drawBall(Ball ball)
{
    // u16 color = ball.color;
    // if (undraw)
    // {
    //     color = BACKGROUND_COLOR;
    // }
    ballSprite->attr0 = (ball.y - 2) | SPRITES_PALETTE_TYPE | TENNIS_BALL_SPRITE_SHAPE;
    ballSprite->attr1 = (ball.x - 2) | TENNIS_BALL_SPRITE_SIZE;
    ballSprite->attr2 = TENNIS_BALL_ID | TENNIS_BALL_PALETTE_ID;

    // drawRectDMA(ball.x, ball.y, ball.size, ball.size, color);
}

int getSwingFrame(int swingFrameCounter)
{
    int frame = (SWING_FRAME_COUNTER_START - swingFrameCounter) / 4;
    if (frame > 2)
    {
        frame = 2;
    }
    return frame;
}

void drawHitBox(Player player)
{
    if (player.racketHitBox.debugColor && player.racketHitBox.enabled)
    {
        HitBox hitBox = player.racketHitBox;
        drawRectDMA(hitBox.x, hitBox.y, hitBox.size, hitBox.size, hitBox.debugColor);
    }
}

void drawPlayer(Player player, int currentlyServing)
{
    if (player.racketHitBox.enabled)
    {
        int frame = getSwingFrame(player.swingFrameCounter);
        playerSprite->attr0 = (player.y - 5) | SPRITES_PALETTE_TYPE | SWING_BLUE_SPRITE_SHAPE;
        playerSprite->attr1 = player.x | SWING_BLUE_SPRITE_SIZE;
        playerSprite->attr2 = swing_blue_frames[frame] | swing_blue_palettes[frame];
    }
    else if (currentlyServing)
    {
        playerSprite->attr0 = (player.y - 5) | SPRITES_PALETTE_TYPE | SERVE_BLUE_SPRITE_SHAPE;
        playerSprite->attr1 = player.x | SERVE_BLUE_SPRITE_SIZE;
        playerSprite->attr2 = SERVE_BLUE_ID | SERVE_BLUE_PALETTE_ID;
    }
    else
    {
        playerSprite->attr0 = (player.y - 5) | SPRITES_PALETTE_TYPE | SWING_BLUE_SPRITE_SHAPE;
        playerSprite->attr1 = player.x | SWING_BLUE_SPRITE_SIZE;
        playerSprite->attr2 = swing_blue_frames[1] | swing_blue_palettes[1];
    }

    drawHitBox(player);
}

void drawCpu(Player cpu, int currentlyServing)
{
    if (cpu.racketHitBox.enabled && !currentlyServing)
    {
        int frame = getSwingFrame(cpu.swingFrameCounter);
        cpuSprite->attr0 = (cpu.y - 5) | SPRITES_PALETTE_TYPE | SWING_RED_SPRITE_SHAPE;
        cpuSprite->attr1 = (cpu.x - PLAYER_WIDTH) | SWING_RED_SPRITE_SIZE;
        cpuSprite->attr2 = swing_red_frames[frame] | swing_red_palettes[frame];
    }
    else if (currentlyServing)
    {
        cpuSprite->attr0 = (cpu.y - 5) | SPRITES_PALETTE_TYPE | SERVE_RED_SPRITE_SHAPE;
        cpuSprite->attr1 = cpu.x | SERVE_RED_SPRITE_SIZE;
        cpuSprite->attr2 = SERVE_RED_ID | SERVE_RED_PALETTE_ID;
    }
    else
    {
        cpuSprite->attr0 = (cpu.y - 5) | SPRITES_PALETTE_TYPE | SWING_RED_SPRITE_SHAPE;
        cpuSprite->attr1 = (cpu.x - PLAYER_WIDTH) | SWING_RED_SPRITE_SIZE;
        cpuSprite->attr2 = swing_red_frames[1] | swing_red_palettes[1];
    }

    drawHitBox(cpu);
}

void drawBallDebug(AppState state)
{
    char str[80];
    sprintf(str, "VelX = %d VelY = %d", state.ball.velX, state.ball.velY);
    drawString(10, 30, str, BLACK);
}

// This function will be used to draw everything about the app
// including the background and whatnot.
void fullDrawAppState(AppState *state)
{
    drawFullScreenImageDMA(tennis_court_background);
    initializeSprites();
    UNUSED(state);
}

// This function will be used to undraw (i.e. erase) things that might
// move in a frame. E.g. in a Snake game, erase the Snake, the food & the score.
void undrawAppState(AppState *state)
{
    if (state->player.racketHitBox.debugColor)
    {
        HitBox hitBox = state->player.racketHitBox;
        drawFullScreenImagePortionDMA(hitBox.x, hitBox.y, hitBox.size, hitBox.size, tennis_court_background);
    }

    if (state->cpu.racketHitBox.debugColor)
    {
        HitBox hitBox = state->cpu.racketHitBox;
        drawFullScreenImagePortionDMA(hitBox.x, hitBox.y, hitBox.size, hitBox.size, tennis_court_background);
    }

    if (state->serveStarted)
    {
        drawFullScreenImagePortionDMA(0, 50, SCREEN_WIDTH, 20, tennis_court_background);
    }

    if (state->ball.landingDebugColor)
    {
        drawFullScreenImagePortionDMA(state->ball.expectedLandingX, GROUND, 5, 5, tennis_court_background);
    }
}

// This function will be used to draw things that might have moved in a frame.
// For example, in a Snake game, draw the snake, the food, the score.
void drawAppState(AppState *state)
{
    drawPlayer(state->player, (state->playerServing && state->serveStarted));
    drawCpu(state->cpu, (state->cpuServing && state->serveStarted));
    drawBall(state->ball);
    drawSprites();

    if (!(state->serveStarted) && (state->playerServing))
    {
        drawCenteredString(0, 50, SCREEN_WIDTH, 20, "Press A to serve", BLACK);
    }
    if (state->ball.landingDebugColor)
    {
        drawRectDMA(state->ball.expectedLandingX, GROUND, 5, 5, state->ball.landingDebugColor);
    }
}
