#ifndef _OBJECTUPDATER_H_
#define _OBJECTUPDATER_H_

#include "inputHandler.h"
#include "objectConstructor.h"
#include "menu.h"
#include "audio.h"
#include <math.h>
#include <time.h>
#include <stdlib.h>

unsigned short uiOldLives;
bool xBaseCollisionTrigger;
bool xBaseCollision;
bool xBaseCollisionOld;

void paddleUpdate(Position *stPadPos, Velocity *stPadVel, Characteristics *stPadChar, Console *stVita, Map *stFrame);
bool ballUpdate(Position *stBallPos, Velocity *stBallVel, Characteristics *stBallChar, Map *stFrame, Position *stPadPos, Velocity *stPadVel, Characteristics *stPadChar, Position *stBlockPos, Characteristics *stBlockChar, Console *stVita, unsigned short uiNrOfBlocks);
void blockUpdate();

#endif