#include <psp2/ctrl.h>
#include <psp2/touch.h>
#include "inputHandler.h"

#define RELEASED 0
#define TOUCHING 1
#define TOUCHED 2
#define RELEASING 3

unsigned short uiRearTouchReport;
unsigned short uiFrontTouchReport;

void startInput(){
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG_WIDE);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_START);
}

void inputRead(Console *stVita) {
    SceCtrlData pad;
    SceTouchData touch;

    sceCtrlPeekBufferPositive(0, &pad, 1);
    stVita->xCircle = (pad.buttons & SCE_CTRL_CIRCLE);
    stVita->xCross = (pad.buttons & SCE_CTRL_CROSS);
    stVita->xTriangle = (pad.buttons & SCE_CTRL_TRIANGLE);
    stVita->xSquare = (pad.buttons & SCE_CTRL_SQUARE);

    stVita->xUp = (pad.buttons & SCE_CTRL_UP);
    stVita->xDown = (pad.buttons & SCE_CTRL_DOWN);
    stVita->xLeft = (pad.buttons & SCE_CTRL_LEFT);
    stVita->xRight = (pad.buttons & SCE_CTRL_RIGHT);

    stVita->xLeftTrigger = (pad.buttons & SCE_CTRL_LTRIGGER);
    stVita->xRightTrigger = (pad.buttons & SCE_CTRL_RTRIGGER);

    stVita->xStart = (pad.buttons & SCE_CTRL_START);
    stVita->xSelect = (pad.buttons & SCE_CTRL_SELECT);

    stVita->stLeftJoy.siX = (signed char)pad.lx - 128;
    stVita->stLeftJoy.siY = (signed char)pad.ly - 128;
    stVita->stRightJoy.siX = (signed char)pad.rx - 128;
    stVita->stRightJoy.siY = (signed char)pad.ry - 128;

    sceTouchPeek(SCE_TOUCH_PORT_BACK, &touch, 1);
    if (touch.reportNum > 0)
    {
        stVita->stRearTouch.uiX = touch.report[0].x / 2;
        stVita->stRearTouch.uiY = touch.report[0].y / 2;
    }
    else
    {
        stVita->stRearTouch.uiX = 0;
        stVita->stRearTouch.uiY = 0;
    }

    stVita->stRearTouch.xFtrig = false;
    stVita->stRearTouch.xRtrig = false;

   switch (uiRearTouchReport)
    {
        case RELEASED:
            if (stVita->stRearTouch.uiX > 0)
            {
                uiRearTouchReport = TOUCHING;
            }
            break;

        case TOUCHING:
            stVita->stRearTouch.xRtrig = true;
            uiRearTouchReport = TOUCHED;
            break;
        
        case TOUCHED:
            if (stVita->stRearTouch.uiX == 0)
            {
                uiRearTouchReport = RELEASING;
            }
            break;

        case RELEASING:
            stVita->stRearTouch.xFtrig = true;
            uiRearTouchReport = RELEASED;
            break;

        default:
            break;
    }

    sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch, 1);
    if (touch.reportNum > 0)
    {
        stVita->stFrontTouch.uiX = touch.report[0].x / 2;
        stVita->stFrontTouch.uiY = touch.report[0].y / 2;
    }
    else
    {
        stVita->stFrontTouch.uiX = 0;
        stVita->stFrontTouch.uiY = 0;
    }    

    stVita->stFrontTouch.xFtrig = false;
    stVita->stFrontTouch.xRtrig = false;

   switch (uiFrontTouchReport)
    {
        case RELEASED:
            if (stVita->stFrontTouch.uiX > 0)
            {
                uiFrontTouchReport = TOUCHING;
            }
            break;

        case TOUCHING:
            stVita->stFrontTouch.xRtrig = true;
            uiFrontTouchReport = TOUCHED;
            break;
        
        case TOUCHED:
            if (stVita->stFrontTouch.uiX == 0)
            {
                uiFrontTouchReport = RELEASING;
            }
            break;

        case RELEASING:
            stVita->stFrontTouch.xFtrig = true;
            uiFrontTouchReport = RELEASED;
            break;

        default:
            break;
    }
}