#include "logic.h"
#include "gba.h"

void initializeAppState(AppState *appState)
{
    const int p_height = 20;
    const int p_width = 10;

    //SETUP PLAYER
    Player *player = &appState->player;
    Player *cpu = &appState->cpu;

    player->body.width = cpu->body.width = p_width;
    player->body.height = cpu->body.height = p_height;

    player->body.color = cpu->body.color = (u16)0x1F;

    player->y = cpu->y = SCREEN_HEIGHT - p_height - 20;
    player->x = 10;
    cpu->x = SCREEN_WIDTH - p_width - 10;
    cpu->isCpu = 1;

    //SETUP HITBOX
    player->racketHitBox.debugColor = cpu->racketHitBox.debugColor = (u16)0x7FF;
    player->racketHitBox.size = cpu->racketHitBox.size = 10;
    player->swingFrameCounter = cpu->swingFrameCounter = -1;

    // START PLAYER AS SERVER
    appState->playerServing = 1;
    appState->serveStarted = 0;
}

void setUpBallForPlayerServe(Ball *ball, Player player)
{
    int offset = (player.isCpu) ? -(player.body.width - 2) : (player.body.width - 2);
    ball->x = player.x + offset;
    ball->y = player.y;
    ball->size = 5;
    ball->color = 0x7FF;
}

void checkForReserve(Player player, Ball ball, int *serveStarted)
{
    if (ball.y > player.y + ball.size)
    {
        *serveStarted = 0;
    }
}

void setBallLocation(Ball *ball)
{
    if (ball->y > SCREEN_HEIGHT)
    {
        ball->velX = 0;
        ball->velY = 0;
        return;
    }
    ball->x += ball->velX;
    ball->velY = APPLY_GRAVITY(ball->velY, vBlankCounter);
    ball->y += ball->velY;
}

void setRacketHitBox(Player *player)
{
    if (player->swingFrameCounter <= 0)
    {
        player->racketHitBox.enabled = 0;
        return;
    }

    player->racketHitBox.x = HIT_BOX_X(player->x, player->swingFrameCounter);
    player->racketHitBox.y = HIT_BOX_Y(player->y, player->swingFrameCounter);

    //decrement counter for next frame
    player->swingFrameCounter--;
}

int racketBallCollision(Player player, Ball *ball)
{
    HitBox hitBox = player.racketHitBox;
    if (!hitBox.enabled)
    {
        return 0;
    }

    int x1 = hitBox.x;
    int x2 = hitBox.x + hitBox.size;
    int y1 = hitBox.y;
    int y2 = hitBox.y + hitBox.size;

    int ballCenterX = ball->x + (ball->size / 2);
    int ballCenterY = ball->y + (ball->size / 2);

    if ((ballCenterX >= x1) && (ballCenterX <= x2) && (ballCenterY >= y1) && (ballCenterY <= y2))
    {
        ball->velX = 3;
        ball->velY = -2;
        return 1;
    }
    return 0;
}

void boing(Ball *ball)
{
    if (ball->y >= GROUND)
    {
        ball->velY *= -1;
    }
}

AppState processAppState(AppState *currentAppState, u32 keysPressedBefore, u32 keysPressedNow)
{
    /* TA-TODO: Do all of your app processing here. This function gets called
     * every frame.
     *
     * To check for key presses, use the KEY_JUST_PRESSED macro for cases where
     * you want to detect each key press once, or the KEY_DOWN macro for checking
     * if a button is still down.
     *
     * To count time, suppose that the GameBoy runs at a fixed FPS (60fps) and
     * that vblank is processed once per frame. Use the vBlankCounter variable
     * and the modulus % operator to do things once every (n) frames. Note that
     * you want to process button every frame regardless (otherwise you will
     * miss inputs.)
     *
     * Do not do any drawing here.
     *
     * TA-TODO: VERY IMPORTANT! READ THIS PART.
     * You need to perform all calculations on the currentAppState passed to you,
     * and perform all state updates on the nextAppState state which we define below
     * and return at the end of the function. YOU SHOULD NOT MODIFY THE CURRENTSTATE.
     * Modifying the currentAppState will mean the undraw function will not be able
     * to undraw it later.
     */

    AppState nextAppState = *currentAppState;

    Player *player = &nextAppState.player;
    Player *cpu = &nextAppState.cpu;
    Ball *ball = &nextAppState.ball;

    if (currentAppState->playerServing)
    {
        if (!currentAppState->serveStarted)
        {
            if (KEY_JUST_PRESSED(BUTTON_B, keysPressedNow, keysPressedBefore))
            {
                ball->velX = 0;
                ball->velY = SERVE_VELOCITY_START;
                nextAppState.serveStarted = 1;
            }
            setUpBallForPlayerServe(ball, currentAppState->player);
            return nextAppState;
        }
    }
    else if (currentAppState->cpuServing)
    {
        if (!currentAppState->serveStarted)
        {
            setUpBallForPlayerServe(ball, currentAppState->cpu);
        }
        nextAppState.serveStarted = 1;
    }
    else //Allow player motion when not serving
    {
        if (KEY_DOWN(BUTTON_RIGHT, BUTTONS) && (player->x < NET_BOUNDARY(player->body.width)))
        {
            player->x += 2;
        }
        if (KEY_DOWN(BUTTON_LEFT, BUTTONS) && (player->x > 0))
        {
            player->x -= 2;
        }
    }
    if (KEY_JUST_PRESSED(BUTTON_A, keysPressedNow, keysPressedBefore) && (player->swingFrameCounter <= 0))
    {
        player->swingFrameCounter = SWING_FRAME_COUNTER_START;
        player->racketHitBox.enabled = 1;
    }

    setRacketHitBox(player);
    setBallLocation(ball);
    checkForReserve(*player, *ball, &nextAppState.serveStarted);
    boing(ball);

    //on racket->ball collision, we assume serving is complete
    if (racketBallCollision(*player, ball))
    {
        nextAppState.playerServing = 0;
    }
    if (racketBallCollision(*cpu, ball))
    {
        nextAppState.cpuServing = 0;
    }

    vBlankCounter++;

    return nextAppState;
}
