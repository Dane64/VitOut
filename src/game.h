#ifndef _GAME_H_
#define _GAME_H_

#include "objectConstructor.h"
#include "objectUpdater.h"
#include "objectVisualizer.h"

void gameLoop(tEnumState *eGameState, stGamePad *stMcd, unsigned char *usiHighScore);

#endif