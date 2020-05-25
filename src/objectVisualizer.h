#ifndef _OBJECTVISUALIZER_H_
#define _OBJECTVISUALIZER_H_

#if (PLATFORM == Vita)
    #include <vita2d.h>
    #include <psp2/display.h>

	#define BLACK		RGBA8(  0,   0,   0, 255)
	#define GREY    	RGBA8(128, 128, 128, 255)
	#define WHITE   	RGBA8(255, 255, 255, 255)
	#define RED     	RGBA8(255,   0,   0, 255)
	#define ORANGE 		RGBA8(255, 165,   0, 255)
	#define YELLOW  	RGBA8(255, 255,   0, 255)
	#define GREEN   	RGBA8(  0, 255,   0, 255)
	#define CYAN    	RGBA8(  0, 255, 255, 255)
	#define BLUE    	RGBA8(  0,   0, 255, 255)
	#define INDIGO  	RGBA8(111,   0, 255, 255)
	#define MAGENTA 	RGBA8(255,   0, 255, 255)

	#define DARKGREY	RGBA8( 51,  51,  51, 255)
	#define DARKRED 	RGBA8( 88,   0,   0, 255)
	#define BROWN   	RGBA8(139,  69,  19, 255)
	#define WHEAT   	RGBA8(245, 222, 179, 255)
	#define PINK    	RGBA8(255, 192, 203, 255)
	#define ALMOND		RGBA8(239, 222, 205, 255)

	#define PLATINUM	RGBA8(229, 228, 226, 255)	
	#define GOLD 		RGBA8(255, 215,   0, 255)
	#define SILVER    	RGBA8(192, 192, 192, 255)
	#define BRONZE    	RGBA8(205, 127,  50, 255)

#endif

#include "objectConstructor.h"

void startVisualizer();
void updateVisualizer();
void showDifficulty(unsigned char usiLevel);
void showHighscore(unsigned short uiHighScore);
void showPaddle(tStObject *stPaddle);
void showBall(tStObject *stBall);
void showFrame(tStObject *stFrame);
void showBrick(tStObject *stBrick, unsigned short uiNumberOfBricks);
void closeVisualizer();
void finishVisualizer();

#endif