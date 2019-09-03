#ifndef _OBJECTUPDATER_H_
#define _OBJECTUPDATER_H_

#include "inputHandler.h"
#include "objectConstructor.h"
#include <math.h>

bool xBaseCollisionTrigger;
bool xBaseCollision;
bool xBaseCollisionOld;

void paddleUpdate(Position *stPadPos, Velocity *stPadVel, Characteristics *stPadChar, Console *stVita, Map *stFrame);
void ballUpdate(Position *stBallPos, Velocity *stBallVel, Characteristics *stBallChar, Map *stFrame, Position *stPadPos, Velocity *stPadVel, Characteristics *stPadChar, Position *stBlockPos, Characteristics *stBlockChar, unsigned short uiNrOfBlocks);
void blockUpdate();

#endif