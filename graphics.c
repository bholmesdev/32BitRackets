#include "graphics.h"
#include "gba.h"

void drawBall(Ball ball, int undraw)
{
    u16 color = ball.color;
    if (undraw)
    {
        color = BACKGROUND_COLOR;
    }

    drawRectDMA(ball.x, ball.y, ball.size, ball.size, color);
}

void drawPlayer(Player player, int undraw)
{
    u16 color = player.body.color;
    if (undraw)
    {
        color = BACKGROUND_COLOR;
    }

    drawRectDMA(player.x, player.y, player.body.width, player.body.height, color);

    if (player.racketHitBox.debugColor)
    {
        HitBox hitBox = player.racketHitBox;
        drawRectDMA(hitBox.x, hitBox.y, hitBox.size, hitBox.size, hitBox.debugColor);
    }
}

// This function will be used to draw everything about the app
// including the background and whatnot.
void fullDrawAppState(AppState *state)
{
    fillScreenDMA(BACKGROUND_COLOR);
    UNUSED(state);
}

// This function will be used to undraw (i.e. erase) things that might
// move in a frame. E.g. in a Snake game, erase the Snake, the food & the score.
void undrawAppState(AppState *state)
{
    drawPlayer(state->player, 1);
    drawPlayer(state->cpu, 1);

    drawBall(state->ball, 1);

    if (state->player.racketHitBox.debugColor)
    {
        HitBox hitBox = state->player.racketHitBox;
        drawRectDMA(hitBox.x, hitBox.y, hitBox.size, hitBox.size, BACKGROUND_COLOR);
    }

    if (state->serveStarted)
    {
        drawCenteredString(0, SCREEN_HEIGHT - 50, SCREEN_WIDTH, 50, "Press B to serve", BACKGROUND_COLOR);
    }
}

// This function will be used to draw things that might have moved in a frame.
// For example, in a Snake game, draw the snake, the food, the score.
void drawAppState(AppState *state)
{
    drawPlayer(state->player, 0);
    drawPlayer(state->cpu, 0);

    drawBall(state->ball, 0);

    if (!(state->serveStarted) && (state->playerServing))
    {
        drawCenteredString(0, SCREEN_HEIGHT - 50, SCREEN_WIDTH, 50, "Press B to serve", BLACK);
    }
}
