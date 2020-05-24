#ifndef _OBJECTCONSTRUCTOR_H_
#define _OBJECTCONSTRUCTOR_H_

#include "inputHandler.h"

#if (PLATFORM == Vita)
	#define WIDTH 960
	#define HEIGHT 544
#elif (PLATFORM == New3DS)
	#define WIDTH 800
	#define HEIGHT 400
#else
	#define WIDTH 0
	#define HEIGHT 0
#endif

#define ROW 20
#define COL 30
#define MAXLEVEL 6
#define MAXLIVES 5

typedef struct tStObject
{
	float rX;
	float rY;

	float rDotX;
	float rDotY;

	unsigned short uiHeight;
	unsigned short uiWidth;
	unsigned short uiLives;
	bool xVisible;

	unsigned int luiColor;
} tStObject;

typedef enum tEnumState{
	BoardSettings,
	LevelSettings,
	DiskSettings,
	MainMenu,
	Credits,
	Quitting,
	Quit,
	Paused,
	Playing,
    LevelOne = 11, // MUST STAY 11 TO CORRECTLY LOAD THE LEVELS
    LevelTwo,
    LevelThree,
    LevelFour,
    LevelFive,
    LevelSix
} tEnumState;

void frameConstructor(tStObject *stFrame);
void paddleConstructor(tStObject *stPad);
void ballConstructor(tStObject *stBall);
void levelConstructor(unsigned char usiLevel, tStObject *stFrame, tStObject *stBrick, unsigned short uiNumberOfBricks);

bool loadGame(unsigned char *usiHighScore);
bool saveGame(unsigned char *usiHighScore);

#endif