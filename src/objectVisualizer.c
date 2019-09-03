#include "objectVisualizer.h"

#define BLACK   RGBA8(  0,   0,   0, 255)
#define WHITE   RGBA8(255, 255, 255, 255)
#define GREEN   RGBA8(  0, 255,   0, 255)
#define RED     RGBA8(255,   0,   0, 255)
#define BLUE    RGBA8(  0,   0, 255, 255)

#define MAX(a, b) ( ( a > b) ? a : b )
#define MIN(a, b) ( ( a < b) ? a : b )

extern unsigned int basicfont_size;
extern unsigned char basicfont[];
vita2d_font *font;

void startVisualizer()
{
    vita2d_init();
    font = vita2d_load_font_mem(basicfont, basicfont_size);
}

void updateVisualizer()
{
	vita2d_start_drawing();
	vita2d_clear_screen();
}

void frameVisualizer(Map *stFrame, Console *Vita, unsigned short uiScreenWidth, unsigned short uiScreenHeight)
{
    vita2d_draw_rectangle(stFrame->uiBottomBorder[1], stFrame->uiBottomBorder[2], uiScreenWidth - stFrame->uiFrameThickness, stFrame->uiFrameThickness, GREEN);
    vita2d_draw_rectangle(stFrame->uiTopBorder[1], stFrame->uiTopBorder[2], uiScreenWidth - stFrame->uiFrameThickness, stFrame->uiFrameThickness, GREEN);
    vita2d_draw_rectangle(stFrame->uiLeftBorder[1], stFrame->uiLeftBorder[2], stFrame->uiFrameThickness, uiScreenHeight, GREEN);
    vita2d_draw_rectangle(stFrame->uiRightBorder[1], stFrame->uiRightBorder[2], stFrame->uiFrameThickness, uiScreenHeight, GREEN);

 /*   vita2d_font_draw_textf(font, 250, 50, WHITE, 25, "TOP: ( %3d, %3d )", stFrame->uiTopBorder[1],stFrame->uiTopBorder[2]);
    vita2d_font_draw_textf(font, 250, 100, WHITE, 25, "BOTTOM: ( %3d, %3d )", stFrame->uiBottomBorder[1],stFrame->uiBottomBorder[2]);
    vita2d_font_draw_textf(font, 250, 150, WHITE, 25, "LEFT: ( %3d, %3d )", stFrame->uiLeftBorder[1],stFrame->uiLeftBorder[2]);
    vita2d_font_draw_textf(font, 250, 200, WHITE, 25, "RIGHT: ( %3d, %3d )", stFrame->uiRightBorder[1],stFrame->uiRightBorder[2]);
    vita2d_font_draw_textf(font, 250, 300, WHITE, 25, "Center: ( %3d, %3d %3d)", uiScreenWidth, uiScreenHeight, stFrame->uiFrameThickness);*/
}

void ballVisualizer(Position *stBallPos, Velocity *stBallVel, Characteristics *stBallChar)
{
	vita2d_draw_fill_circle(stBallPos->rX, stBallPos->rY, stBallChar->uiHeight, WHITE);
//    vita2d_font_draw_textf(font, 10, 525, WHITE, 25, "Ball Position: ( %f, %f )", stBallPos->rX, stBallPos->rY);
//    vita2d_font_draw_textf(font, 10, 525, WHITE, 25, "Ball Speed: ( %f, %f )", stBallVel->rDotX, stBallVel->rDotY);
}

void paddleVisualizer(Position *stPadPos, Characteristics *stPadChar)
{
	vita2d_draw_rectangle(stPadPos->rX, stPadPos->rY, stPadChar->uiWidth, stPadChar->uiHeight, RED);
//	vita2d_font_draw_textf(font, 600, 525, WHITE, 25, "Paddle Position: ( %f, %f )", stPadPos->rX, stPadPos->rY);
}

void blockVisualizer(Position *stBlockPos, Characteristics *stBlockChar, unsigned short uiNrOfBlocks)
{
    unsigned short i = 0;
    for (i = 0; i < uiNrOfBlocks; i++)
    {
        if (stBlockChar[i].xVisible)
        {
            vita2d_draw_rectangle(stBlockPos[i].rX, stBlockPos[i].rY, stBlockChar[i].uiWidth, stBlockChar[i].uiHeight, BLUE);
        }
    }
}

void closeVisualizer()
{
	vita2d_end_drawing();
	vita2d_swap_buffers();
	sceDisplayWaitVblankStart();
}

void finishVisualizer()
{
    vita2d_fini();
}