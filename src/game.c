#include "game.h"

#define BRICKS 600

tStObject stFrame[4];
tStObject stBall;
tStObject stPaddle;
tStObject stBrick[BRICKS];

tEnumState ePrvGameState = MainMenu;
bool xInit = true;

void gameLoop(tEnumState *eGameState, stGamePad *stMcd, unsigned char *usiHighScore)
{
	static unsigned char usiLevelLoaded;
	static float tCum;

	if (ePrvGameState != *eGameState && *eGameState >= Playing)
	{
		xInit = true;
	}

	if (xInit)
	{
		if (*eGameState >= Playing)
		{
			frameConstructor(stFrame);
			ballConstructor(&stBall);
			paddleConstructor(&stPaddle);
		}
		usiLevelLoaded = *eGameState - 10;
		levelConstructor(usiLevelLoaded, stFrame, stBrick, BRICKS);
		xInit = false;
	}

	float tDelta = getElapsedtime();

	if (*eGameState != Paused)
	{
		if (*eGameState >= Playing)
		{
			paddleUpdate(stMcd, &stPaddle, stFrame, tDelta);
			ballUpdate(stMcd, &stBall, &stPaddle, stFrame, stBrick, BRICKS, tDelta);
		}
		tCum = brickUpdate(*eGameState, stBrick, BRICKS, tDelta);
	}

	if (*eGameState >= Paused)
	{
		showFrame(stFrame);
		showPaddle(&stPaddle);
		showBall(&stBall);
	}

	if (stMcd->stButt[7].xTrigger && *eGameState >= Paused)
	{
		if (*eGameState >= Playing)
		{
			*eGameState = Paused;
		}
		else
		{
			*eGameState = Playing;
		}
	}

	showBrick(stBrick, BRICKS);
	ePrvGameState = *eGameState;

	if (stBall.uiLives == 0 || checkBricks(stBrick, BRICKS))
	{
		xInit = true;
		if (usiHighScore[usiLevelLoaded-1] < getHighScore(usiLevelLoaded, stBall.uiLives, tCum))
		{
			usiHighScore[usiLevelLoaded-1] = getHighScore(usiLevelLoaded, stBall.uiLives, tCum);
			saveGame(usiHighScore);
		}

		stBall.uiLives = -1;		// gameLoop is getting called every cycle, set lives to -1 to prevent constantly switching back to MainMenu
		*eGameState = MainMenu;
	}
}