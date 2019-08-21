#include <stdio.h>

#include <psp2/kernel/processmgr.h>

#include "inputHandler.h"
#include "objectConstructor.h"
#include "objectUpdater.h"
#include "objectVisualizer.h"

int main() {
	unsigned short uiScreenWidth = 960;
	unsigned short uiScreenHeight = 544;

	Base stBall;
	Base stPaddle;
	Map stFrame;
	Console stVita;

	frameConstructor(&stFrame, uiScreenWidth, uiScreenHeight);
	paddleConstructor(&stPaddle);
	ballConstructor(&stBall);
	startVisualizer();

	while (1) {
		inputHandler(&stVita);
		paddleUpdate(&stPaddle, &stVita);
		ballUpdate(&stBall, &stFrame);

		updateVisualizer();
		frameVisualizer(&stFrame, &stVita, uiScreenWidth, uiScreenHeight);
		paddleVisualizer(&stPaddle);
		ballVisualizer(&stBall);

		closeVisualizer();
	}

	finishVisualizer();
	sceKernelExitProcess(0);
	return 0;
}
