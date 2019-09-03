#include <stdio.h>

#include <psp2/kernel/processmgr.h>

#include "inputHandler.h"
#include "objectConstructor.h"
#include "objectUpdater.h"
#include "objectVisualizer.h"

#define NROFBLOCKS 100

int main() {
	unsigned short uiScreenWidth = 960;
	unsigned short uiScreenHeight = 544;

	Position stBallPos;
	Position stPadPos;
	Position stBlockPos[NROFBLOCKS];

	Velocity stBallVel;
	Velocity stPadVel;

	Characteristics stBallChar;
	Characteristics stPadChar;
	Characteristics stBlockChar[NROFBLOCKS];

	Map stFrame;
	Console stVita;

	frameConstructor(&stFrame, uiScreenWidth, uiScreenHeight);
	paddleConstructor(&stPadPos, &stPadVel, &stPadChar);
	ballConstructor(&stBallPos, &stBallVel, &stBallChar);
	blockConstructor(stBlockPos, stBlockChar, &stFrame, NROFBLOCKS);
	startVisualizer();

	while (1) {
		inputHandler(&stVita);
		paddleUpdate(&stPadPos, &stPadVel, &stPadChar, &stVita, &stFrame);
		ballUpdate(&stBallPos, &stBallVel, &stBallChar, &stFrame, &stPadPos, &stPadVel, &stPadChar, stBlockPos,stBlockChar, NROFBLOCKS);

		updateVisualizer();
		frameVisualizer(&stFrame, &stVita, uiScreenWidth, uiScreenHeight);
		paddleVisualizer(&stPadPos, &stPadChar);
		blockVisualizer(stBlockPos, stBlockChar, NROFBLOCKS);
		ballVisualizer(&stBallPos, &stBallVel, &stBallChar);

		closeVisualizer();
	}

	finishVisualizer();
	sceKernelExitProcess(0);
	return 0;
}
