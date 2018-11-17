#include "gba.h"
#include "sprites.h"

volatile unsigned short *videoBuffer = (volatile unsigned short *)0x6000000;
u32 vBlankCounter = 0;
u32 gravityCounter = 0;

unsigned short swing_red_palettes[3] =
    {
        SWING_RED0_PALETTE_ID,
        SWING_RED1_PALETTE_ID,
        SWING_RED2_PALETTE_ID};

unsigned short swing_blue_palettes[3] =
    {
        SWING_BLUE0_PALETTE_ID,
        SWING_BLUE1_PALETTE_ID,
        SWING_BLUE2_PALETTE_ID};

volatile OamEntry shadow[128];

void waitForVBlank(void)
{
    while (*SCANLINECOUNTER > 160)
        ;
    while (*SCANLINECOUNTER < 160)
        ;
}

static int __qran_seed = 42;
static int qran(void)
{
    __qran_seed = 1664525 * __qran_seed + 1013904223;
    return (__qran_seed >> 16) & 0x7FFF;
}

int randint(int min, int max)
{
    return (qran() * (max - min) >> 15) + min;
}

void setPixel(int x, int y, u16 color)
{
    videoBuffer[OFFSET(y, x, SCREEN_WIDTH)] = color;
}

void drawRectDMA(int x, int y, int width, int height, volatile u16 color)
{
    for (int row = 0; row < height; row++)
    {
        DMA[3].src = &color;
        DMA[3].dst = &videoBuffer[OFFSET(y + row, x, SCREEN_WIDTH)];
        DMA[3].cnt = width | DMA_SOURCE_FIXED | DMA_DESTINATION_INCREMENT | DMA_ON;
    }
}

void drawFullScreenImageDMA(const u16 *image)
{
    DMA[3].src = image;
    DMA[3].dst = videoBuffer;
    DMA[3].cnt = (SCREEN_HEIGHT * SCREEN_WIDTH) | DMA_SOURCE_INCREMENT | DMA_DESTINATION_INCREMENT | DMA_ON;
}

void drawImageDMA(int x, int y, int width, int height, u16 *image)
{
    for (int row = 0; row < height; row++)
    {
        DMA[3].src = &image[row * width];
        DMA[3].dst = &videoBuffer[OFFSET(y + row, x, SCREEN_WIDTH)];
        DMA[3].cnt = width | DMA_SOURCE_INCREMENT | DMA_DESTINATION_INCREMENT | DMA_ON;
    }
}

void fillScreenDMA(volatile u16 color)
{
    DMA[3].src = &color;
    DMA[3].dst = videoBuffer;
    DMA[3].cnt = SCREEN_HEIGHT * SCREEN_WIDTH | DMA_SOURCE_FIXED | DMA_ON;
}

void initializeSprites(void)
{
    DMA[3].src = sprites_palette;
    DMA[3].dst = SPRITEPAL;
    DMA[3].cnt = SPRITES_PALETTE_LENGTH | DMA_ON;

    DMA[3].src = sprites;
    DMA[3].dst = &CHARBLOCKBASE[5];
    DMA[3].cnt = SPRITES_LENGTH | DMA_ON;

    for (int i = 0; i < 128; i++)
    {
        shadow[i].attr0 = ATTR0_HIDE;
    }
}

void drawSprites(void)
{
    DMA[3].src = shadow;
    DMA[3].dst = OAMMEM;
    DMA[3].cnt = 128 * 4 | DMA_ON;
}

void drawChar(int col, int row, char ch, u16 color)
{
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 6; c++)
        {
            if (fontdata_6x8[OFFSET(r, c, 6) + ch * 48])
            {
                setPixel(col + c, row + r, color);
            }
        }
    }
}

void drawString(int col, int row, char *str, u16 color)
{
    while (*str)
    {
        drawChar(col, row, *str++, color);
        col += 6;
    }
}

void drawCenteredString(int x, int y, int width, int height, char *str, u16 color)
{
    u32 len = 0;
    char *strCpy = str;
    while (*strCpy)
    {
        len++;
        strCpy++;
    }

    u32 strWidth = 6 * len;
    u32 strHeight = 8;

    int col = x + ((width - strWidth) >> 1);
    int row = y + ((height - strHeight) >> 1);
    drawString(col, row, str, color);
}
