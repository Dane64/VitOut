#include "menu.h"
#include "game.h"

stGamePad stMcd;
tEnumState eGameState;
unsigned char usiHighScore[MAXLEVEL];

int main()
{
	startVisualizer();
	startInput();
	loadGame(usiHighScore);
	eGameState = MainMenu;

	while (eGameState != Quit)
	{
		inputRead(&stMcd);
		updateVisualizer();

		gameLoop(&eGameState, &stMcd, usiHighScore);
		mainMenu(&eGameState, &stMcd, usiHighScore);

		closeVisualizer();
	}

	finishVisualizer();
	sceKernelExitProcess(0);
	return 0;
}
