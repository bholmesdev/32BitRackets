#include "logic.h"
#include "gba.h"

void initializePlayers(Player *player, Player *cpu)
{
    player->width = cpu->width = PLAYER_WIDTH;
    player->height = cpu->height = PLAYER_HEIGHT;
    player->racketHitBox.enabled = cpu->racketHitBox.enabled = 0;
    player->swingFrameCounter = cpu->swingFrameCounter = 0;

    player->y = cpu->y = GROUND - PLAYER_HEIGHT;
    player->x = 20;
    player->velJump = cpu->velJump = 0;
    cpu->x = SCREEN_WIDTH - PLAYER_WIDTH - 20;
    cpu->isCpu = 1;
}

void initializeAppState(AppState *appState)
{
    //SETUP PLAYER
    Player *player = &appState->player;
    Player *cpu = &appState->cpu;
    Ball *ball = &appState->ball;
    Score *score = &appState->score;
    initializePlayers(player, cpu);

    //SETUP HITBOX
    player->racketHitBox.size = cpu->racketHitBox.size = 12;
    player->swingFrameCounter = cpu->swingFrameCounter = 0;
    // player->racketHitBox.debugColor = cpu->racketHitBox.debugColor = CYAN;

    //SETUP BALL
    ball->size = BALL_SIZE;
    // ball->landingDebugColor = 0x7FF;

    //ZERO OUT SCORES
    score->player = 0;
    score->cpu = 0;

    // START PLAYER AS SERVER
    appState->playerServing = 1;
    appState->serveStarted = 0;
}

void setScore(Player advantagePlayer, Score *score)
{
    if (advantagePlayer.isCpu)
    {
        score->cpu++;
    }
    else
    {
        score->player++;
    }
    if (((score->player == 4) && (score->cpu == 4))) // Both players have deuce advantage
    {
        score->player = score->cpu = 3;
    }
    if ((score->player == 4) && (score->cpu < 3)) // Player wins
    {
        score->player = 5;
    }
    if ((score->cpu == 4) && (score->player < 3)) // Cpu wins
    {
        score->cpu = 5;
    }

    //TODO: Remove these lines
    if (score->player > 5)
        score->player = 5;

    if (score->cpu > 5)
        score->cpu = 5;
}

void addTextToDisplayQueue(char *text, int durationCounter, TextDisplay **textDisplayQueue)
{
    TextDisplay *textDisplay = (TextDisplay *)malloc(sizeof(TextDisplay));
    textDisplay->text = text;
    textDisplay->durationCounter = durationCounter;
    textDisplay->next = *textDisplayQueue;
    *textDisplayQueue = textDisplay;
}

void displayOut(TextDisplay **textDisplay)
{
    addTextToDisplayQueue("Out", 60, textDisplay);
}

void popFromTextDisplayQueue(TextDisplay **textDisplayQueue)
{
    if (textDisplayQueue != NULL)
    {
        TextDisplay *oldHead = *textDisplayQueue;
        *textDisplayQueue = (*textDisplayQueue)->next;
        free(oldHead);
    }
}

void setUpBallForPlayerServe(Ball *ball, Player player)
{
    int offset = (player.isCpu) ? -(player.width - 4) : (player.width - 4);
    ball->x = player.x + offset;
    ball->y = player.y;
    ball->velX = ball->velY = 0;
    ball->expectedLandingX = 0;
}

void reinitializeServe(Player player, Ball ball, AppState *state)
{
    if (ball.y > player.y + ball.size)
    {
        state->serveStarted = 0;
    }

    if ((ball.hasBounced && ball.x > SCREEN_WIDTH) || (!ball.hasBounced && (ball.x < COURT_EDGE_LEFT)))
    {
        state->serveStarted = 0;
        state->playerServing = 1;
        setScore(state->player, &state->score);
        if (!ball.hasBounced) //if there was no bounce, it must be out
        {
            displayOut(&state->textDisplayQueue);
        }
    }
    else if ((!ball.hasBounced && (ball.x > COURT_EDGE_RIGHT)) || (ball.hasBounced && ball.x < 0))
    {
        state->serveStarted = 0;
        state->playerServing = 1;
        setScore(state->cpu, &state->score);
        if (!ball.hasBounced) //if there was no bounce, it must be out
        {
            displayOut(&state->textDisplayQueue);
        }
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
    ball->velY = APPLY_BALL_GRAVITY(ball->velY, gravityCounter);
    ball->y += ball->velY;
}

int getBallLandingX(Ball ball, Player player)
{
    int horizontalTravel = 0;
    if (ball.velX == 3)
    {
        horizontalTravel = 170;
    }
    else if (ball.velX == 2)
    {
        horizontalTravel = 140;
    }
    else
    {
        horizontalTravel = 85;
    }
    int expectedBallLandingX = ball.x + horizontalTravel + PLAYER_JUMP_CPU_POS_FACTOR(player.y, ball.velX) - BALL_SPEED_CPU_POS_FACTOR(ball.velX);
    ;
    if (!expectedBallLandingX || (expectedBallLandingX > COURT_EDGE_RIGHT)) // if it will be out, return to center court
    {
        expectedBallLandingX = (SCREEN_WIDTH * 3) / 4;
    }
    if (expectedBallLandingX < NET_BOUNDARY_RIGHT + PLAYER_WIDTH) // if the ball is close to the net, go for the bounce
    {
        expectedBallLandingX = COURT_EDGE_RIGHT;
    }
    return expectedBallLandingX;
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
    if (expectedBallLandingX == 0)
    {
        expectedBallLandingX = 3 * SCREEN_WIDTH / 4; // go to center court by default
    }
    int diff = expectedBallLandingX - cpu->x;
    if ((expectedBallLandingX) && ((diff < -1) || (diff > 1)))
    {
        cpu->x += (diff < 0) ? -1 : 1;
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

    if (player->y + player->height > GROUND)
    {
        player->jumpGravityCounter = 0;
    }
}

int boing(Ball *ball)
{
    if (ball->y - ball->size >= GROUND)
    {
        ball->velY *= -1;
        ball->y += ball->velY;
        return 1;
    }
    return 0;
}

AppState processAppState(AppState *currentAppState, u32 keysPressedBefore, u32 keysPressedNow)
{
    AppState nextAppState = *currentAppState;

    Player *player = &nextAppState.player;
    Player *cpu = &nextAppState.cpu;
    Ball *ball = &nextAppState.ball;

    if (currentAppState->textDisplayQueue != NULL)
    {
        nextAppState.textDisplayQueue->durationCounter--;
        if ((*currentAppState->textDisplayQueue).durationCounter <= 0)
        {
            popFromTextDisplayQueue(&nextAppState.textDisplayQueue);
        }
        initializePlayers(player, cpu);
        return nextAppState;
    }

    int ballComingTowardsPlayer = (ball->velX <= 0);

    if (boing(ball)) // check for out on ball bounce
    {
        ball->hasBounced = 1;
        if (ballComingTowardsPlayer && (ball->x > NET_BOUNDARY_LEFT))
        {
            displayOut(&nextAppState.textDisplayQueue);
            setScore(*player, &nextAppState.score);
            initializePlayers(player, cpu);
            setUpBallForPlayerServe(ball, currentAppState->player);
            nextAppState.serveStarted = 0;
            nextAppState.playerServing = 1;
            return nextAppState;
        }
        if (!ballComingTowardsPlayer && (ball->x < NET_BOUNDARY_RIGHT))
        {
            displayOut(&nextAppState.textDisplayQueue);
            setScore(*cpu, &nextAppState.score);
            initializePlayers(player, cpu);
            setUpBallForPlayerServe(ball, currentAppState->player);
            nextAppState.serveStarted = 0;
            nextAppState.playerServing = 1;
            return nextAppState;
        }
    }

    if (currentAppState->playerServing)
    {
        if (!currentAppState->serveStarted)
        {
            initializePlayers(player, cpu);
            setUpBallForPlayerServe(ball, currentAppState->player);
            if (KEY_JUST_PRESSED(BUTTON_B, keysPressedNow, keysPressedBefore))
            {
                ball->velX = 0;
                ball->velY = SERVE_VELOCITY_START;
                nextAppState.serveStarted = 1;
                gravityCounter = 0; //reset gravity on new serve
            }
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
        if (KEY_DOWN(BUTTON_RIGHT, BUTTONS) && (player->x + player->width < NET_BOUNDARY_LEFT))
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
    //attempt CPU swing when ball gets close
    if ((currentAppState->cpuSwingDelay > SWING_DELAY_MIN) && cpu->x - ball->x - currentAppState->cpuSwingDelay <= 0)
    {
        cpu->swingFrameCounter = SWING_FRAME_COUNTER_START;
        cpu->racketHitBox.enabled = 1;
        nextAppState.cpuSwingDelay = SWING_DELAY_MIN;
    }

    setRacketHitBox(player);
    setRacketHitBox(cpu);
    moveCpuToBall(cpu, ball->expectedLandingX);
    setBallLocation(ball);
    reinitializeServe(*player, *ball, &nextAppState);
    jump(player);

    // on racket->ball collision, we assume serving is complete
    if (racketBallCollision(*player, ball))
    {
        nextAppState.ball.expectedLandingX = getBallLandingX(*ball, *player);
        nextAppState.cpuSwingDelay = RANDOMIZED_SWING_TIMING(vBlankCounter, ball->velX); //set up cpu to swing
        if (nextAppState.ball.expectedLandingX >= SCREEN_WIDTH - PLAYER_WIDTH)
        {
            nextAppState.cpuSwingDelay /= 2;
        }
        nextAppState.playerServing = 0;
        ball->hasBounced = 0;
    }
    if (racketBallCollision(*cpu, ball))
    {
        nextAppState.cpuServing = 0;
        nextAppState.ball.expectedLandingX = 0;
    }

    gravityCounter++;
    vBlankCounter++;

    return nextAppState;
}
