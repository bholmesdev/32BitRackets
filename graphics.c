#include "graphics.h"
#include "gba.h"
#include "sprites.h"
#include <stdio.h>

volatile OamEntry *playerSprite = &shadow[0];
volatile OamEntry *cpuSprite = &shadow[1];

void drawBall(Ball ball, int undraw)
{
    u16 color = ball.color;
    if (undraw)
    {
        color = BACKGROUND_COLOR;
    }

    drawRectDMA(ball.x, ball.y, ball.size, ball.size, color);
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
    fillScreenDMA(BACKGROUND_COLOR);
    initializeSprites();
    UNUSED(state);
}

// This function will be used to undraw (i.e. erase) things that might
// move in a frame. E.g. in a Snake game, erase the Snake, the food & the score.
void undrawAppState(AppState *state)
{
    drawBall(state->ball, 1);

    if (state->player.racketHitBox.debugColor)
    {
        HitBox hitBox = state->player.racketHitBox;
        drawRectDMA(hitBox.x, hitBox.y, hitBox.size, hitBox.size, BACKGROUND_COLOR);
    }

    if (state->cpu.racketHitBox.debugColor)
    {
        HitBox hitBox = state->cpu.racketHitBox;
        drawRectDMA(hitBox.x, hitBox.y, hitBox.size, hitBox.size, BACKGROUND_COLOR);
    }

    if (state->serveStarted)
    {
        drawCenteredString(0, SCREEN_HEIGHT - 50, SCREEN_WIDTH, 50, "Press A to serve", BACKGROUND_COLOR);
    }

    // drawRectDMA(10, 20, SCREEN_WIDTH, 30, BACKGROUND_COLOR);
    drawRectDMA(state->expectedBallLandingX, SCREEN_HEIGHT - 20, 5, 5, BACKGROUND_COLOR);
}

// This function will be used to draw things that might have moved in a frame.
// For example, in a Snake game, draw the snake, the food, the score.
void drawAppState(AppState *state)
{
    drawPlayer(state->player, (state->playerServing && state->serveStarted));
    drawCpu(state->cpu, (state->cpuServing && state->serveStarted));
    drawSprites();

    /*
    OamEntry* dog = shadow;
	dog->attr0 = 48 | DOG_PALETTE_TYPE | DOG_SPRITE_SHAPE;
	dog->attr1 = 88 | DOG_SPRITE_SIZE;
	dog->attr2 = DOG_PALETTE_ID | DOG_ID;

    */

    drawBall(state->ball, 0);

    if (!(state->serveStarted) && (state->playerServing))
    {
        drawCenteredString(0, SCREEN_HEIGHT - 50, SCREEN_WIDTH, 50, "Press A to serve", BLACK);
    }
    // drawBallDebug(*state);
    drawRectDMA(state->expectedBallLandingX, SCREEN_HEIGHT - 20, 5, 5, GREEN);
}
