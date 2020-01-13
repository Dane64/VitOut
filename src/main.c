#include <stdio.h>
#include <psp2/kernel/processmgr.h>
#include "menu.h"
#include "game.h"
#include "audio.h"
#include "inputHandler.h"
#include "splashScreen.h"

int main() {
	unsigned short uiScreenWidth = 960;
	unsigned short uiScreenHeight = 544;
	unsigned short uiStartLevel = 0;
	
	Console stVita;

	startInput();
	startVisualizer();
	startAudio();
	splash();

	while (1) {
		inputRead(&stVita);
		updateVisualizer();
		if (uiStartLevel == 0)
		{
			uiStartLevel = drawMenuItems(&stVita, uiScreenWidth, uiScreenHeight);
		}
		
		if (uiStartLevel >= 1)
		{
			uiStartLevel = engine(&stVita, uiScreenWidth, uiScreenHeight, uiStartLevel);
		}
		
		closeVisualizer();
	}

	finishVisualizer();
	sceKernelExitProcess(0);
	return 0;
}
