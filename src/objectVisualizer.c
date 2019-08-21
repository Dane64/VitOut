#include "objectVisualizer.h"

#define BLACK   RGBA8(  0,   0,   0, 255)
#define WHITE   RGBA8(255, 255, 255, 255)
#define GREEN   RGBA8(  0, 255,   0, 255)
#define RED     RGBA8(255,   0,   0, 255)
#define BLUE    RGBA8(  0,   0, 255, 255)

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

void ballVisualizer(Base *stBall)
{
	vita2d_draw_fill_circle(stBall->rXPos, stBall->rYPos, 20, WHITE);
    vita2d_font_draw_textf(font, 10, 525, WHITE, 25, "Ball Position: ( %1f, %1f )", stBall->rXPos, stBall->rYPos);
}

void paddleVisualizer(Base *stPaddle)
{
	vita2d_draw_rectangle(stPaddle->rXPos, stPaddle->rYPos, 180, 30, RED);
	vita2d_font_draw_textf(font, 600, 525, WHITE, 25, "Paddle Position: ( %3f, %3f )", stPaddle->rXPos, stPaddle->rYPos);
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