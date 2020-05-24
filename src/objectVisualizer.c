#include "objectVisualizer.h"

void startVisualizer()
{
	vita2d_init_advanced_with_msaa(1048576, SCE_GXM_MULTISAMPLE_4X);
	vita2d_set_vblank_wait(1);
	vita2d_set_clear_color(GREY);
}

void updateVisualizer()
{
	vita2d_start_drawing();
	vita2d_clear_screen();
}

void showHighscore(unsigned short uiHighScore)
{
	for (int i=1; i <4; i++)
	{
		if (uiHighScore >= i)
		{
			vita2d_draw_fill_circle(WIDTH-(25*i), HEIGHT-25, 10, BRONZE);
		}
		else
		{
			vita2d_draw_fill_circle(WIDTH-(25*i), HEIGHT-25, 10, WHITE);
		}
	}
}

void showDifficulty(unsigned char usiLevel)
{
	unsigned char i;

	for (i = 1; i < usiLevel+1; i++)
	{
		vita2d_draw_rectangle(WIDTH-(20*i), HEIGHT-75, 15, 15, RED);
	}
}

void showPaddle(tStObject *stPaddle)
{
	unsigned char usiRedTerm = stPaddle->luiColor & 0xFF;
	unsigned char usiGreenTerm = ((stPaddle->luiColor)>>8) & 0xFF;
	unsigned char usiBlueTerm = ((stPaddle->luiColor)>>16) & 0xFF;

	unsigned int luiDarkerColor = ((((255)&0xFF)<<24) | (((usiBlueTerm/7)&0xFF)<<16) | (((usiGreenTerm/7)&0xFF)<<8) | (((usiRedTerm/7)&0xFF)<<0));

	vita2d_draw_fill_circle(stPaddle->rX+stPaddle->uiHeight/2, stPaddle->rY+stPaddle->uiHeight/2, stPaddle->uiHeight/2, luiDarkerColor);
	vita2d_draw_fill_circle(stPaddle->rX+stPaddle->uiWidth-stPaddle->uiHeight/2, stPaddle->rY+stPaddle->uiHeight/2, stPaddle->uiHeight/2, luiDarkerColor);
	vita2d_draw_rectangle(stPaddle->rX+stPaddle->uiHeight/2, stPaddle->rY, stPaddle->uiWidth-stPaddle->uiHeight, stPaddle->uiHeight, stPaddle->luiColor);
	vita2d_draw_rectangle(stPaddle->rX+stPaddle->uiHeight, stPaddle->rY, 5, stPaddle->uiHeight, luiDarkerColor);
	vita2d_draw_rectangle(stPaddle->rX+stPaddle->uiWidth-stPaddle->uiHeight-5, stPaddle->rY, 5, stPaddle->uiHeight, luiDarkerColor);
	// vita2d_draw_line(stPaddle->rX+stPaddle->uiHeight/2, stPaddle->rY+stPaddle->uiHeight*1/6, stPaddle->rX+stPaddle->uiWidth-stPaddle->uiHeight/2, stPaddle->rY+stPaddle->uiHeight*1/6, luiDarkerColor);
	// vita2d_draw_line(stPaddle->rX+stPaddle->uiHeight/2, stPaddle->rY+stPaddle->uiHeight*5/6, stPaddle->rX+stPaddle->uiWidth-stPaddle->uiHeight/2, stPaddle->rY+stPaddle->uiHeight*5/6, luiDarkerColor);
}

void showBall(tStObject *stBall)
{
	vita2d_draw_fill_circle(stBall->rX, stBall->rY, stBall->uiHeight, stBall->luiColor);

	//Show lives
	for (int i=1; i <MAXLIVES+1; i++)
	{
		if (stBall->uiLives >= i)
		{
			vita2d_draw_fill_circle(WIDTH-(18*i), 5, 4, stBall->luiColor);
		}
		else
		{
			vita2d_draw_fill_circle(WIDTH-(18*i), 5, 4, WHITE);
		}
	}

}

void showFrame(tStObject *stFrame)
{
	if (stFrame[0].xVisible)
	{
		vita2d_draw_rectangle(stFrame[0].rX, stFrame[0].rY, stFrame[0].uiWidth, stFrame[0].uiHeight, stFrame[0].luiColor);
	}
	
	if (stFrame[1].xVisible)
	{
		vita2d_draw_rectangle(stFrame[1].rX, stFrame[1].rY, stFrame[1].uiWidth, stFrame[1].uiHeight, stFrame[1].luiColor);
	}

	if (stFrame[2].xVisible)
	{
		vita2d_draw_rectangle(stFrame[2].rX, stFrame[2].rY, stFrame[2].uiWidth, stFrame[3].uiHeight, stFrame[2].luiColor);
	}

	if (stFrame[3].xVisible)
	{
		vita2d_draw_rectangle(stFrame[3].rX, stFrame[3].rY, stFrame[3].uiWidth, stFrame[3].uiHeight, stFrame[3].luiColor);
	}
}

void showBrick(tStObject *stBrick, unsigned short uiNumberOfBricks)
{
	for (int i=0; i<uiNumberOfBricks; i++)
	{
		if (stBrick[i].xVisible)
		{
			unsigned char usiRedTerm = stBrick[i].luiColor & 0xFF;
			unsigned char usiGreenTerm = ((stBrick[i].luiColor)>>8) & 0xFF;
			unsigned char usiBlueTerm = ((stBrick[i].luiColor)>>16) & 0xFF;

			unsigned int luiDarkerColor = ((((255)&0xFF)<<24) | (((usiBlueTerm/stBrick[i].uiLives)&0xFF)<<16) | (((usiGreenTerm/stBrick[i].uiLives)&0xFF)<<8) | (((usiRedTerm/stBrick[i].uiLives)&0xFF)<<0));

			vita2d_draw_rectangle(stBrick[i].rX, stBrick[i].rY, stBrick[i].uiWidth, stBrick[i].uiHeight, stBrick[i].luiColor);
			vita2d_draw_rectangle(stBrick[i].rX+stBrick[i].uiWidth*0.1, stBrick[i].rY+stBrick[i].uiHeight*0.15, stBrick[i].uiWidth*0.8, stBrick[i].uiHeight*0.7, luiDarkerColor);
		}
	}
}

void closeVisualizer()
{
    vita2d_wait_rendering_done();
	vita2d_end_drawing();
	vita2d_swap_buffers();
	sceDisplayWaitVblankStart();
}

void finishVisualizer()
{
    vita2d_fini();
}