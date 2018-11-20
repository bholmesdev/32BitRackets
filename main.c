#include "gba.h"
#include "logic.h"
#include "graphics.h"
#include "sprites.h"
#include "images/splashScreen.h"
#include "images/rulesScreen.h"
#include "images/winnerScreen.h"

#include <stdio.h>
#include <stdlib.h>

// AppState enum definition
typedef enum
{
    START,
    START_WAIT_FOR_INPUT,
    WIPE_ANIM,
    RULES,
    RULES_WAIT_FOR_INPUT,
    APP_INIT,
    APP,
    APP_EXIT,
    APP_EXIT_NODRAW,
} GBAState;

int main(void)
{
    REG_DISPCNT = MODE3 | BG2_ENABLE | OBJ_ENABLE | SPRITES_DIMENSION_TYPE;

    GBAState state = START;

    // We store the "previous" and "current" states.
    AppState currentAppState, nextAppState;

    // We store the current and previous values of the button input.
    u32 previousButtons = BUTTONS;
    u32 currentButtons = BUTTONS;

    int blinkCounter = 0;
    int wipeAnimationCounter = 0;

    int playerWon = 0;

    while (1)
    {
        // Load the current state of the buttons
        currentButtons = BUTTONS;

        // TA-TODO: Manipulate the state machine below as needed.
        switch (state)
        {
        case START:
            waitForVBlank();
            hideSprites();
            drawFullScreenImageDMA(splash_screen);

            state = START_WAIT_FOR_INPUT;
            break;
        case START_WAIT_FOR_INPUT:
            blinkCounter++;
            waitForVBlank();
            if (blinkCounter % 50 >= 25)
            {
                drawCenteredString(0, 131, SCREEN_WIDTH, 10, "Press Start", BLACK);
                drawCenteredString(0, 130, SCREEN_WIDTH, 10, "Press Start", YELLOW);
            }
            else
            {
                drawFullScreenImagePortionDMA(0, 130, SCREEN_WIDTH, 10, splash_screen);
            }
            if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons))
            {
                state = RULES;
            }

            break;
        case RULES:
            waitForVBlank();
            drawFullScreenImageDMA(rules_background);
            drawCenteredString(0, 25, SCREEN_WIDTH, 10, "Press A to jump", WHITE);
            drawCenteredString(0, 60, SCREEN_WIDTH, 10, "Press B to swing", WHITE);
            drawCenteredString(0, 70, SCREEN_WIDTH, 10, "(lead your shots!)", WHITE);
            drawCenteredString(0, 100, SCREEN_WIDTH, 10, "Press B to serve", WHITE);
            state = RULES_WAIT_FOR_INPUT;
            break;
        case RULES_WAIT_FOR_INPUT:
            blinkCounter++;
            waitForVBlank();
            if (blinkCounter % 50 >= 25)
            {
                drawCenteredString(0, 136, SCREEN_WIDTH, 10, "Press Start to Play!", BLACK);
                drawCenteredString(0, 135, SCREEN_WIDTH, 10, "Press Start to Play!", YELLOW);
            }
            else
            {
                drawFullScreenImagePortionDMA(0, 135, SCREEN_WIDTH, 10, rules_background);
            }
            if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons))
            {
                wipeAnimationCounter = 0;
                state = WIPE_ANIM;
            }
            break;
        case WIPE_ANIM:
            waitForVBlank();
            if (wipeAnimationCounter < 240)
            {
                drawFullScreenImagePortionDMA(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT - wipeAnimationCounter, rules_background);
                drawRectDMA(0, SCREEN_HEIGHT - wipeAnimationCounter, SCREEN_WIDTH, wipeAnimationCounter, BLACK);
                wipeAnimationCounter += 6;
            }
            else
            {
                state = APP_INIT;
            }
            break;
        case APP_INIT:
            // Initialize the app. Switch to the APP state.
            initializeAppState(&currentAppState);

            // Draw the initial state of the app
            fullDrawAppState(&currentAppState);

            state = APP;
            break;
        case APP:
            // Process the app for one frame, store the next state
            nextAppState = processAppState(&currentAppState, previousButtons, currentButtons);

            // Wait for vblank before we do any drawing.
            waitForVBlank();

            // Undraw the previous state
            undrawAppState(&currentAppState);

            // Draw the current state
            drawAppState(&nextAppState);

            // Now set the current state as the next state for the next iter.
            currentAppState = nextAppState;

            if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons))
            {
                state = START;
            }

            if (nextAppState.playerMatchWinner || nextAppState.cpuMatchWinner)
            {
                playerWon = nextAppState.playerMatchWinner;
                state = APP_EXIT;
            }

            break;
        case APP_EXIT:
            // Wait for vblank
            waitForVBlank();

            drawFullScreenImageDMA(winner_background);
            hideSprites();

            if (playerWon)
            {
                drawCenteredString(0, 1, SCREEN_WIDTH, SCREEN_HEIGHT, "Nice serves Player!", BLACK);
                drawCenteredString(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, "Nice serves Player!", WHITE);
            }
            else
            {
                drawCenteredString(0, 1, SCREEN_WIDTH, SCREEN_HEIGHT, "Better luck next time!", BLACK);
                drawCenteredString(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, "Better luck next time!", WHITE);
            }

            drawCenteredString(0, SCREEN_HEIGHT - 30, SCREEN_WIDTH, 10, "Press Start for a Rematch", BLACK);
            drawCenteredString(0, SCREEN_HEIGHT - 31, SCREEN_WIDTH, 10, "Press Start for a Rematch", YELLOW);

            state = APP_EXIT_NODRAW;
            break;
        case APP_EXIT_NODRAW:
            if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons))
            {
                state = START;
            }

            break;
        }

        // Store the current state of the buttons
        previousButtons = currentButtons;
    }

    return 0;
}
