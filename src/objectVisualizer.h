#ifndef _OBJECTVISUALIZER_H_
#define _OBJECTVISUALIZER_H_

#include <psp2/display.h>
#include <vita2d.h>

#include "objectUpdater.h"

void startVisualizer();
void updateVisualizer();
void frameVisualizer(Map *stFrame, Console *Vita, unsigned short uiScreenWidth, unsigned short uiScreenHeight);
void ballVisualizer(Base *stBall);
void paddleVisualizer(Base *stPaddle);
void closeVisualizer();
void finishVisualizer();

#endif