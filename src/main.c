#include "menu.h"
#include "game.h"

stGamePad stMcd;
tEnumState eGameState;

int main()
{
	startVisualizer();
	startInput();
	eGameState = MainMenu;

	while (eGameState != Quit)
	{
		inputRead(&stMcd);
		updateVisualizer();

		gameLoop(&eGameState, &stMcd);
		mainMenu(&eGameState, &stMcd);

		closeVisualizer();
	}

	finishVisualizer();
	sceKernelExitProcess(0);
	return 0;
}
