#ifndef _OBJECTUPDATER_H_
#define _OBJECTUPDATER_H_

#include "objectConstructor.h"
#include <vitasdk.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/rtc.h>

void paddleUpdate(stGamePad *stMcd, tStObject *stPaddle, tStObject *stFrame);
void ballUpdate(stGamePad *stMcd, tStObject *stBall, tStObject *stPaddle, tStObject *stFrame, tStObject *stBrick, unsigned short uiNumberOfBricks);
void brickUpdate(tStObject *stBrick, unsigned short uiNumberOfBricks);

#endif