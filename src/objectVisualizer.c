#include "objectVisualizer.h"

#define BLACK   RGBA8(  0,   0,   0, 255)
#define GREY    RGBA8(111, 111, 111, 255)
#define WHITE   RGBA8(255, 255, 255, 255)
#define RED     RGBA8(255,   0,   0, 255)
#define ORANGE  RGBA8(255, 165,   0, 255)
#define YELLOW  RGBA8(255, 255,   0, 255)
#define GREEN   RGBA8(  0, 255,   0, 255)
#define CYAN    RGBA8(  0, 255, 255, 255)
#define BLUE    RGBA8(  0,   0, 255, 255)
#define INDIGO  RGBA8(111,   0, 255, 255)
#define MAGENTA RGBA8(255,   0, 255, 255)

#define BROWN   RGBA8(139,  69,  19, 255)
#define WHEAT   RGBA8(245, 222, 179, 255)
#define PINK    RGBA8(255, 192, 203, 255)
#define KHAKI   RGBA8(240, 230, 140, 255)

#define MAX(a, b) ( ( a > b) ? a : b )
#define MIN(a, b) ( ( a < b) ? a : b )

void frameVisualizer(Map *stFrame, Console *stVita, unsigned short uiScreenWidth, unsigned short uiScreenHeight, bool xGamePaused)
{
//    vita2d_draw_rectangle(stFrame->uiBottomBorder[1], stFrame->uiBottomBorder[2], uiScreenWidth - stFrame->uiFrameThickness, stFrame->uiFrameThickness, GREEN);
    vita2d_draw_rectangle(stFrame->uiTopBorder[1], stFrame->uiTopBorder[2], uiScreenWidth - stFrame->uiFrameThickness, stFrame->uiFrameThickness, GREEN);
    vita2d_draw_rectangle(stFrame->uiLeftBorder[1], stFrame->uiLeftBorder[2], stFrame->uiFrameThickness, uiScreenHeight, GREEN);
    vita2d_draw_rectangle(stFrame->uiRightBorder[1], stFrame->uiRightBorder[2], stFrame->uiFrameThickness, uiScreenHeight, GREEN);

    if(xGamePaused)
    {
        vita2d_font_draw_text(font, 250, 325, RED, 150, "PAUSED");
    }
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
    vita2d_font_draw_textf(font, 850, 35, WHITE, 25, "Lives: %i", stBallChar->uiLives);

    if(stBallChar->uiLives == 0)
    {
        vita2d_font_draw_text(font, 75, 325, RED, 150, "GAME OVER");
    }
}

void paddleVisualizer(Position *stPadPos, Velocity *stPadVel, Characteristics *stPadChar)
{
	vita2d_draw_rectangle(stPadPos->rX, stPadPos->rY, stPadChar->uiWidth, stPadChar->uiHeight, RED);
//	vita2d_font_draw_textf(font, 600, 525, WHITE, 25, "Paddle Position: ( %f, %f )", stPadPos->rX, stPadPos->rY);
//    vita2d_font_draw_textf(font, 600, 525, WHITE, 25, "Paddle Speed: ( %f, %f )", stPadVel->rDotX, stPadVel->rDotY);
}

void blockVisualizer(Position *stBlockPos, Characteristics *stBlockChar, unsigned short uiNrOfBlocks)
{
    unsigned short i = 0;
    for (i = 0; i < uiNrOfBlocks; i++)
    {
        if (stBlockChar[i].xVisible)
        {
            if (stBlockChar[i].uiLives == 6)
            {
                vita2d_draw_rectangle(stBlockPos[i].rX, stBlockPos[i].rY, stBlockChar[i].uiWidth, stBlockChar[i].uiHeight, BLACK);
            }
            else if (stBlockChar[i].uiLives == 5)
            {
                vita2d_draw_rectangle(stBlockPos[i].rX, stBlockPos[i].rY, stBlockChar[i].uiWidth, stBlockChar[i].uiHeight, WHEAT);
            }
            else if (stBlockChar[i].uiLives == 4)
            {
                vita2d_draw_rectangle(stBlockPos[i].rX, stBlockPos[i].rY, stBlockChar[i].uiWidth, stBlockChar[i].uiHeight, WHITE);
            }
            else if (stBlockChar[i].uiLives == 3)
            {
                vita2d_draw_rectangle(stBlockPos[i].rX, stBlockPos[i].rY, stBlockChar[i].uiWidth, stBlockChar[i].uiHeight, PINK);
            }
            else if (stBlockChar[i].uiLives == 2)
            {
                vita2d_draw_rectangle(stBlockPos[i].rX, stBlockPos[i].rY, stBlockChar[i].uiWidth, stBlockChar[i].uiHeight, BROWN);
            }
            else if (stBlockChar[i].uiLives == 1)
            {
                vita2d_draw_rectangle(stBlockPos[i].rX, stBlockPos[i].rY, stBlockChar[i].uiWidth, stBlockChar[i].uiHeight, KHAKI);
            }
        }
    }
}