#ifndef _OBJECTVISUALIZER_H_
#define _OBJECTVISUALIZER_H_

#include <psp2/display.h>
#include <vita2d.h>

#include "objectUpdater.h"

void frameVisualizer(Map *stFrame, Console *stVita, unsigned short uiScreenWidth, unsigned short uiScreenHeight, bool xGamePaused);
void ballVisualizer(Position *stBallPos, Velocity *stBallVel, Characteristics *stBallChar);
void paddleVisualizer(Position *stPadPos, Velocity *stPadVel, Characteristics *stPadChar);
void blockVisualizer(Position *stBlockPos, Characteristics *stBlockChar, unsigned short uiNrOfBlocks);

#endif