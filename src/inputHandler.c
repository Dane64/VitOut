#include <psp2/ctrl.h>
#include <psp2/touch.h>
#include "inputHandler.h"

void inputHandler(Console *stVita) {
    SceCtrlData pad;
    SceTouchData touch;

    sceCtrlPeekBufferPositive(0, &pad, 1);
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG_WIDE);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_START);

    stVita->xCircle = (pad.buttons & SCE_CTRL_CIRCLE);
    stVita->xCross = (pad.buttons & SCE_CTRL_CROSS);
    stVita->xTriangle = (pad.buttons & SCE_CTRL_TRIANGLE);
    stVita->xSquare = (pad.buttons & SCE_CTRL_SQUARE);
    stVita->stLeftJoy.siX = (signed char)pad.lx - 128;
    stVita->stLeftJoy.siY = (signed char)pad.ly - 128;
    stVita->stRightJoy.siX = (signed char)pad.rx - 128;
    stVita->stRightJoy.siY = (signed char)pad.ry - 128;

}