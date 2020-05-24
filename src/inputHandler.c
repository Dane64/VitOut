#include <psp2/ctrl.h>
#include <psp2/touch.h>
#include "inputHandler.h"

void startInput(){
    if (PLATFORM == Vita)
        sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG_WIDE);
        sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
        sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_START);
}

void inputRead(stGamePad *stMcd) {
        SceCtrlData stPad;
        SceTouchData stTouch; // TODO Not allowed here, must be in vita function
    if (PLATFORM == Vita) // Button layout borrowed from DS3 controller
        sceCtrlPeekBufferPositive(0, &stPad, 1);

        stMcd->stButt[0].xTrigger = !stMcd->stButt[0].xHold && (stPad.buttons & SCE_CTRL_SQUARE);
        stMcd->stButt[0].xHold = (stPad.buttons & SCE_CTRL_SQUARE);

        stMcd->stButt[1].xTrigger = !stMcd->stButt[1].xHold && (stPad.buttons & SCE_CTRL_CROSS);
        stMcd->stButt[1].xHold = (stPad.buttons & SCE_CTRL_CROSS);

        stMcd->stButt[2].xTrigger = !stMcd->stButt[2].xHold && (stPad.buttons & SCE_CTRL_CIRCLE);
        stMcd->stButt[2].xHold = (stPad.buttons & SCE_CTRL_CIRCLE);

        stMcd->stButt[3].xTrigger = !stMcd->stButt[3].xHold && (stPad.buttons & SCE_CTRL_TRIANGLE);
        stMcd->stButt[3].xHold = (stPad.buttons & SCE_CTRL_TRIANGLE);

        stMcd->stButt[4].xTrigger = !stMcd->stButt[4].xHold && (stPad.buttons & SCE_CTRL_LTRIGGER);
        stMcd->stButt[4].xHold = (stPad.buttons & SCE_CTRL_LTRIGGER);

        stMcd->stButt[5].xTrigger = !stMcd->stButt[5].xHold && (stPad.buttons & SCE_CTRL_RTRIGGER);
        stMcd->stButt[5].xHold = (stPad.buttons & SCE_CTRL_RTRIGGER);

        stMcd->stButt[6].xTrigger = !stMcd->stButt[6].xHold && (stPad.buttons & SCE_CTRL_SELECT);
        stMcd->stButt[6].xHold = (stPad.buttons & SCE_CTRL_SELECT);

        stMcd->stButt[7].xTrigger = !stMcd->stButt[7].xHold && (stPad.buttons & SCE_CTRL_START);
        stMcd->stButt[7].xHold = (stPad.buttons & SCE_CTRL_START);

        stMcd->stDpad[0].xTrigger = (!(stMcd->stDpad[0].xUp || stMcd->stDpad[0].xDown || stMcd->stDpad[0].xLeft || stMcd->stDpad[0].xRight) &&
                                    ((stPad.buttons & SCE_CTRL_UP) || (stPad.buttons & SCE_CTRL_DOWN) || (stPad.buttons & SCE_CTRL_LEFT) || (stPad.buttons & SCE_CTRL_RIGHT)));                                    
        stMcd->stDpad[0].xUp = (stPad.buttons & SCE_CTRL_UP);
        stMcd->stDpad[0].xDown = (stPad.buttons & SCE_CTRL_DOWN);
        stMcd->stDpad[0].xLeft = (stPad.buttons & SCE_CTRL_LEFT);
        stMcd->stDpad[0].xRight = (stPad.buttons & SCE_CTRL_RIGHT);

        stMcd->stJoy[0].siX = stPad.lx > 127 ? limit(100,0, stPad.lx-137) : limit(0,-100, stPad.lx-117);
        stMcd->stJoy[0].siY = stPad.ly > 127 ? limit(100,0, stPad.ly-137) : limit(0,-100, stPad.ly-117);

        stMcd->stJoy[1].siX = stPad.rx > 127 ? limit(100,0, stPad.rx-137) : limit(0,-100, stPad.rx-117);
        stMcd->stJoy[1].siY = stPad.ry > 127 ? limit(100,0, stPad.ry-137) : limit(0,-100, stPad.ry-117);

        sceTouchPeek(SCE_TOUCH_PORT_FRONT, &stTouch, 1);
        stMcd->stTouch[0].xTrigger = (stMcd->stTouch[0].uiX == 0) && (stTouch.report[0].x > 0);
        stMcd->stTouch[0].uiX = (stTouch.reportNum > 0) ? stTouch.report[0].x/2 : 0;
        stMcd->stTouch[0].uiY = (stTouch.reportNum > 0) ? stTouch.report[0].y/2 : 0;

        sceTouchPeek(SCE_TOUCH_PORT_BACK, &stTouch, 1);
        stMcd->stTouch[1].xTrigger = (stMcd->stTouch[1].uiX == 0) && (stTouch.report[0].x > 0);
        stMcd->stTouch[1].uiX = (stTouch.reportNum > 0) ? stTouch.report[0].x/2 : 0;
        stMcd->stTouch[1].uiY = (stTouch.reportNum > 0) ? stTouch.report[0].y/2 : 0;
}