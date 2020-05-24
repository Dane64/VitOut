#ifndef _OBJECTUPDATER_H_
#define _OBJECTUPDATER_H_

#include "objectConstructor.h"
#include <vitasdk.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/rtc.h>

float getElapsedtime();
void paddleUpdate(stGamePad *stMcd, tStObject *stPaddle, tStObject *stFrame, float tDelta);
void ballUpdate(stGamePad *stMcd, tStObject *stBall, tStObject *stPaddle, tStObject *stFrame, tStObject *stBrick, unsigned short uiNumberOfBricks, float tDelta);
void brickUpdate(tEnumState eGameState, tStObject *stBrick, unsigned short uiNumberOfBricks, float tDelta);
bool checkBricks(tStObject *stBrick, unsigned short uiNumberOfBricks);

#endif