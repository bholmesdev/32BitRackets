/*
 * Exported with nin10kit v1.7
 * Invocation command was nin10kit --mode=sprites --bpp=4 --for_bitmap --transparent=000000 sprites ./images/swing_red.gif ./images/swing_blue.gif ./images/serve_red.png ./images/serve_blue.png ./images/tennis_ball.png 
 * Time-stamp: Sunday 11/18/2018, 11:39:40
 * 
 * Image Information
 * -----------------
 * ./images/swing_red.gif (frame 0) 32@32
 * ./images/swing_red.gif (frame 1) 32@32
 * ./images/swing_red.gif (frame 2) 32@32
 * ./images/swing_blue.gif (frame 0) 32@32
 * ./images/swing_blue.gif (frame 1) 32@32
 * ./images/swing_blue.gif (frame 2) 32@32
 * ./images/serve_red.png 32@32
 * ./images/serve_blue.png 32@32
 * ./images/tennis_ball.png 8@8
 * Transparent color: (0, 0, 0)
 * 
 * All bug reports / feature requests are to be filed here https://github.com/TricksterGuy/nin10kit/issues
 */

#ifndef SPRITES_H
#define SPRITES_H

#define SPRITES_PALETTE_TYPE (0 << 13)
#define SPRITES_DIMENSION_TYPE (1 << 6)

extern const unsigned short sprites_palette[256];
#define SPRITES_PALETTE_SIZE 512
#define SPRITES_PALETTE_LENGTH 256

extern const unsigned short sprites[2064];
#define SPRITES_SIZE 4128
#define SPRITES_LENGTH 2064

#define SWING_RED0_PALETTE_ID (0 << 12)
#define SWING_RED0_ID 512

#define SWING_RED1_PALETTE_ID (1 << 12)
#define SWING_RED1_ID 528

#define SWING_RED2_PALETTE_ID (2 << 12)
#define SWING_RED2_ID 544

#define SWING_BLUE0_PALETTE_ID (3 << 12)
#define SWING_BLUE0_ID 560

#define SWING_BLUE1_PALETTE_ID (4 << 12)
#define SWING_BLUE1_ID 576

#define SWING_BLUE2_PALETTE_ID (5 << 12)
#define SWING_BLUE2_ID 592

#define SERVE_RED_PALETTE_ID (6 << 12)
#define SERVE_RED_SPRITE_SHAPE (0 << 14)
#define SERVE_RED_SPRITE_SIZE (2 << 14)
#define SERVE_RED_ID 608

#define SERVE_BLUE_PALETTE_ID (7 << 12)
#define SERVE_BLUE_SPRITE_SHAPE (0 << 14)
#define SERVE_BLUE_SPRITE_SIZE (2 << 14)
#define SERVE_BLUE_ID 624

#define TENNIS_BALL_PALETTE_ID (8 << 12)
#define TENNIS_BALL_SPRITE_SHAPE (0 << 14)
#define TENNIS_BALL_SPRITE_SIZE (0 << 14)
#define TENNIS_BALL_ID 640

extern const unsigned short swing_red_frames[3];
#define SWING_RED_FRAMES 3

#define SWING_RED_SPRITE_SHAPE (0 << 14)
#define SWING_RED_SPRITE_SIZE (2 << 14)

extern const unsigned short swing_blue_frames[3];
#define SWING_BLUE_FRAMES 3

#define SWING_BLUE_SPRITE_SHAPE (0 << 14)
#define SWING_BLUE_SPRITE_SIZE (2 << 14)

#endif

