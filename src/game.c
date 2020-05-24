#include "game.h"

#define BRICKS 600

tStObject stFrame[4];
tStObject stBall;
tStObject stPaddle;
tStObject stBrick[BRICKS];

tEnumState ePrvGameState = MainMenu;
bool xInit = true;

void gameLoop(tEnumState *eGameState, stGamePad *stMcd)
{
	if (ePrvGameState != *eGameState)
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
		levelConstructor(*eGameState - 10, stFrame, stBrick, BRICKS);
		xInit = false;
	}

	if (*eGameState != Paused)
	{
		paddleUpdate(stMcd, &stPaddle, stFrame);
		ballUpdate(stMcd, &stBall, &stPaddle, stFrame, stBrick, BRICKS);
		brickUpdate(stBrick, BRICKS);
	}

	if (*eGameState >= Playing)
	{
		showPaddle(&stPaddle);
		showBall(&stBall);
		showFrame(stFrame);
	}

	showBrick(stBrick, BRICKS);

	ePrvGameState = *eGameState;
}