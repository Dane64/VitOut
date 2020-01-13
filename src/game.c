#include "game.h"

#define NROFBLOCKS 600

    Position stBallPos;
	Position stPadPos;
	Position stBlockPos[NROFBLOCKS];

	Velocity stBallVel;
	Velocity stPadVel;

	Characteristics stBallChar;
	Characteristics stPadChar;
	Characteristics stBlockChar[NROFBLOCKS];

	Map stFrame;

	unsigned short uiInit = 1;

	bool xGameDone;
	bool xOldButtonState;
	bool xGamePaused;

int engine(/*GameState *eGame,*/ Console *stVita, unsigned short uiScreenWidth, unsigned short uiScreenHeight, unsigned short uiStartLevel) {	
    
	if (uiInit == 1)
	{
		frameConstructor(&stFrame, uiScreenWidth, uiScreenHeight);
		paddleConstructor(&stPadPos, &stPadVel, &stPadChar);
		ballConstructor(&stBallPos, &stBallVel, &stBallChar);
		levelConstructor(stBlockPos, stBlockChar, &stFrame, uiStartLevel, NROFBLOCKS);
//		blockConstructor(stBlockPos, stBlockChar, &stFrame, NROFBLOCKS);
		loadAudio();
		uiInit = 0;
	}

	if (stVita->xStart && !xOldButtonState && !xGamePaused)
	{
		xGamePaused = true;
	}
	else if (stVita->xStart && !xOldButtonState && xGamePaused)
	{
		xGamePaused = false;
	}

	xOldButtonState = stVita->xStart;

	if(xGamePaused || stBallChar.uiLives == 0)
	{
		xGameDone = stVita->xCircle;
	}
	else
	{
		paddleUpdate(&stPadPos, &stPadVel, &stPadChar, stVita, &stFrame);
		xGameDone = ballUpdate(&stBallPos, &stBallVel, &stBallChar, &stFrame, &stPadPos, &stPadVel, &stPadChar, stBlockPos, stBlockChar, stVita, NROFBLOCKS);
	}

	frameVisualizer(&stFrame, stVita, uiScreenWidth, uiScreenHeight, xGamePaused);
	paddleVisualizer(&stPadPos, &stPadVel, &stPadChar);
	blockVisualizer(stBlockPos, stBlockChar, NROFBLOCKS);
	ballVisualizer(&stBallPos, &stBallVel, &stBallChar);

	if(xGameDone)
	{
		uiInit = 1;
		return 0;
	}

	return 1;

}