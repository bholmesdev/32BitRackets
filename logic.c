#include "logic.h"
#include "gba.h"

void initializePlayers(Player *player, Player *cpu)
{
    const int p_height = 20;
    const int p_width = 10;

    player->body.width = cpu->body.width = p_width;
    player->body.height = cpu->body.height = p_height;

    player->body.color = cpu->body.color = RED;

    player->y = cpu->y = SCREEN_HEIGHT - p_height - 20;
    player->x = 20;
    player->velJump = cpu->velJump = 0;
    cpu->x = SCREEN_WIDTH - p_width - 20;
    cpu->isCpu = 1;
}

void initializeAppState(AppState *appState)
{

    //SETUP PLAYER
    Player *player = &appState->player;
    Player *cpu = &appState->cpu;
    initializePlayers(player, cpu);

    //SETUP HITBOX
    player->racketHitBox.debugColor = cpu->racketHitBox.debugColor = CYAN;
    player->racketHitBox.size = cpu->racketHitBox.size = 12;
    player->swingFrameCounter = cpu->swingFrameCounter = 0;

    // START PLAYER AS SERVER
    appState->playerServing = 1;
    appState->serveStarted = 0;
}

void setUpBallForPlayerServe(Ball *ball, Player player)
{
    int offset = (player.isCpu) ? -(player.body.width - 4) : (player.body.width - 4);
    ball->x = player.x + offset;
    ball->y = player.y;
    ball->size = 5;
    ball->color = 0x7FF;
}

void checkForReServe(Player player, Ball ball, AppState *state)
{
    if (ball.y > player.y + ball.size)
    {
        state->serveStarted = 0;
    }
    // if ((ball.x > SCREEN_WIDTH) || (ball.x < 0))
    // {
    //     state->serveStarted = 0;
    //     state->playerServing = 1;
    // }
}

void setBallLocation(Ball *ball)
{
    if (ball->y > SCREEN_HEIGHT)
    {
        ball->velX = 0;
        ball->velY = 0;
        return;
    }
    if ((ball->x > SCREEN_WIDTH) || (ball->x < 0))
    {
        ball->velX *= -1;
        ball->x += ball->velX;
    }
    ball->x += ball->velX;
    ball->velY = APPLY_BALL_GRAVITY(ball->velY, gravityCounter);
    ball->y += ball->velY;
}

int getBallLandingX(Ball ball)
{
    int horizontalTravel = 0;
    if (ball.velX == 3)
    {
        horizontalTravel = 150;
    }
    else if (ball.velX == 2)
    {
        horizontalTravel = 140;
    }
    else
    {
        horizontalTravel = 85;
    }
    return ball.x + horizontalTravel;
}

void setRacketHitBox(Player *player)
{
    if (player->swingFrameCounter == 0)
    {
        player->racketHitBox.enabled = 0;
        return;
    }

    player->racketHitBox.x = player->isCpu ? HIT_BOX_X_CPU(player->x, player->swingFrameCounter) : HIT_BOX_X(player->x, player->swingFrameCounter);
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
        gravityCounter = 0;
        ball->velX = player.isCpu ? BALL_CPU_VEL_X(hitBox.x, player.x) : BALL_VEL_X(hitBox.x, player.x);
        ball->velY = BALL_VEL_Y(hitBox.y, player.y);
        return 1;
    }
    return 0;
}

void moveCpuToBall(Player *cpu, int expectedBallLandingX)
{
    int diff = expectedBallLandingX - cpu->x;
    if ((expectedBallLandingX) && ((diff < -1) || (diff > 1)))
    {
        cpu->x += (diff < 0) ? -2 : 2;
    }
}

void jump(Player *player)
{
    if (player->jumpGravityCounter == 0)
    {
        return;
    }
    player->velJump = APPLY_JUMP_GRAVITY(player->velJump, player->jumpGravityCounter);

    player->y += player->velJump;
    player->jumpGravityCounter++;

    if (player->y + player->body.height > GROUND)
    {
        player->jumpGravityCounter = 0;
    }
}

void boing(Ball *ball)
{
    if (ball->y - ball->size >= GROUND)
    {
        ball->velY *= -1;
        ball->y += ball->velY;
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

    int ballComingTowardsPlayer = (ball->velX <= 0);

    if (currentAppState->playerServing)
    {
        if (!currentAppState->serveStarted)
        {
            if (KEY_JUST_PRESSED(BUTTON_B, keysPressedNow, keysPressedBefore))
            {
                ball->velX = 0;
                ball->velY = SERVE_VELOCITY_START;
                nextAppState.serveStarted = 1;
                gravityCounter = 0; //reset gravity on new serve
            }
            initializePlayers(player, cpu);
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
        if (KEY_JUST_PRESSED(BUTTON_A, keysPressedNow, keysPressedBefore) && (player->jumpGravityCounter == 0))
        {
            player->velJump = -2;
            player->jumpGravityCounter++;
        }
    }
    if (KEY_JUST_PRESSED(BUTTON_B, keysPressedNow, keysPressedBefore) && (!player->racketHitBox.enabled) && (ballComingTowardsPlayer))
    {
        player->swingFrameCounter = SWING_FRAME_COUNTER_START;
        player->racketHitBox.enabled = 1;
    }
    if ((currentAppState->expectedBallLandingX - ball->x < 10) && (cpu->swingFrameCounter == 0) && (currentAppState->expectedBallLandingX > 0)) //attempt CPU swing when it gets close
    {
        cpu->swingFrameCounter = SWING_FRAME_COUNTER_START;
        cpu->racketHitBox.enabled = 1;
    }

    setRacketHitBox(player);
    setRacketHitBox(cpu);
    moveCpuToBall(cpu, currentAppState->expectedBallLandingX);
    setBallLocation(ball);
    checkForReServe(*player, *ball, &nextAppState);
    boing(ball);
    jump(player);

    //on racket->ball collision, we assume serving is complete
    if (racketBallCollision(*player, ball))
    {
        nextAppState.playerServing = 0;
        nextAppState.expectedBallLandingX = getBallLandingX(*ball) + PLAYER_JUMP_CPU_POS_FACTOR(player->y) - BALL_SPEED_CPU_POS_FACTOR(ball->velX);
        if (nextAppState.expectedBallLandingX > SCREEN_WIDTH - 5)
        {
            nextAppState.expectedBallLandingX = SCREEN_WIDTH - 20;
        }
    }
    if (racketBallCollision(*cpu, ball))
    {
        nextAppState.cpuServing = 0;
        nextAppState.expectedBallLandingX = 0;
    }

    gravityCounter++;

    return nextAppState;
}
